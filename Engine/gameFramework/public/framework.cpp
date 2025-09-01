#include "base/registeredInfo.h"
#include <framework.h>

void GActor::loop(float deltatime, GameWindow &window_) {
  for (GComponent *comp : __allComponents) {
    comp->loop(window_, deltatime);
    comp->tick();
  }
}
void GActor::setActive() { isActive = true; };
void GActor::disableActive() { isActive = false; };
void GActor::setPositionWs(const FVector3 &posWs_) {
  if (nodeId == -1) {
    positionWs = posWs_;
    return;
  }
  int nodeIdTemp = getWorld()->gridMap.getPositionIndex(posWs_);
  if (nodeIdTemp == -1)
    return;
  if (nodeIdTemp != nodeId) {
    getWorld()->gridMap.changeActorNode(this, nodeIdTemp, nodeId);
    nodeId = nodeIdTemp;
  }

  positionWs = posWs_;
};
///////////////////////////////////////////////////////////////////////////////////////////
FVector3 GSceneComponent::getPositionWs() {
  if (parentComp) {
    return parentComp->getPositionWs() + positionRelative;
  }
  if (owner)
  {
    return owner->getPositionWs() + positionRelative;
  }
  return {0,0,0};
}
void GSceneComponent::setPositionWs(const FVector3 &posWs_) {
  setPositionRelative(posWs_ - owner->getPositionWs());
}
///////////////////////////////////////////////////////////////////////////////////////////
void GRenderObjComponent::loop(GameWindow &window_, float deltaTime_) {
  owner->getWorld()->getRenderObjComps().push_back(this);
  sprite->posWs = getPositionWs();
}
///////////////////////////////////////////////////////////////////////////////////////////
GPlayer::GPlayer() {
  moveComp = createComponent<GMoveComponent>();
  moveComp->speed = 1;
  cameraComp = createComponent<GCameraComponent>();
  sprComp = createComponent<GStaticSpriteComponent>();
  controller.bind(GController::a, [this]() {
    moveComp->moveX = -1;
    moveComp->isAutoMove = false;
  });
  controller.bind(GController::d, [this]() {
    moveComp->moveX = 1;
    moveComp->isAutoMove = false;
  });
  controller.bind(GController::w, [this]() {
    moveComp->moveY = -1;
    moveComp->isAutoMove = false;
  });
  controller.bind(GController::s, [this]() {
    moveComp->moveY = 1;
    moveComp->isAutoMove = false;
  });
  controller.bind(GController::mleft, [this]() {
    FVector3 target =
        cameraComp->camera.getMousePositionWs(getWorld()->gm.gameIns->window);
    moveComp->setTarget(target);
  });
  controller.bind(GController::kup, [this]() {
    if (cameraComp->camera.getPixSize() < 0.0001)
      return;
    cameraComp->camera.setPixSize(cameraComp->camera.getPixSize() - 0.0001);
  });
  controller.bind(GController::kdown, [this]() {
    cameraComp->camera.setPixSize(cameraComp->camera.getPixSize() + 0.0001);
  });
}

void GPlayer::beginPlay() {
  sprComp->setTex(
      GSource::getSource().getObject("res/arr_110110_c_5_5_0.5_1_tree.png"));
  sprComp->getSprite().setId(10);
  sprComp->setSizeWs({2, 2, 0});
}
///////////////////////////////////////////////////////////////////////////////////////////
void GWorld::loadBaseActors(const std::string &jsonPath_) {
  std::thread th([&]() { asyncLoad(jsonPath_); });
  th.detach();

  pageWait.loop(gm.gameIns->window, gm.gameIns->event, isDataLoadComplete);
}
GStaticActor *GWorld::createStaticActor(const std::string &name_,
                                        const FVector3 &pos_,
                                        const FVector3 &sizeWs_) {
  auto actor = createActor<GStaticActor>(pos_);

  GStaticActor::Info &staticActorInfo = ClassInfo::getStaticActorInfo(name_);
  actor->infoPtr = &staticActorInfo;
  actor->construct(GSource::getSource().getObject(staticActorInfo.texPath),
                   staticActorInfo.texIndex);
  actor->sprComp->setSizeWs(sizeWs_);
  return actor;
}
void GWorld::asyncLoad(const std::string &jsonPath_) {
  nlohmann::json jsobj;
  std::ifstream ifile(jsonPath_);
  ifile >> jsobj;
  ifile.close();
  // 初始化地图
  gridMap.init(jsobj["mapInfo"]["row"], jsobj["mapInfo"]["column"],
               jsobj["mapInfo"]["width"], jsobj["mapInfo"]["height"]);
  // 绑定默认控制器和相机：相机需要地图信息，所以在这里绑定
  bindDefaultCameraController();
  // staticActor
  for (auto info : jsobj["staticActors"]) {

    const std::string &name = info["name"];
    const std::vector<float> &pos = info["position"].get<std::vector<float>>();
    FVector3 position = {pos[0], pos[1], pos[2]};
    const std::vector<float> &size = info["sizeWs"].get<std::vector<float>>();
    FVector3 sizeWs = {size[0], size[1], size[2]};

    createStaticActor(name, position, sizeWs);
  }
  // animActor
  for (auto info : jsobj["animActors"]) {
    // int texId = info["texId"];
    size_t texId = Gstring::calculateHash(info["path"].get<std::string>());
    int beginIndex = info["beginIndex"];
    int endIndex = info["endIndex"];
    int frameSpeed = info["frameSpeed"];
    const std::vector<float> &pos = info["position"].get<std::vector<float>>();
    FVector3 position = {pos[0], pos[1], pos[2]};
    const std::vector<float> &size = info["sizeWs"].get<std::vector<float>>();
    FVector3 sizeWs = {size[0], size[1], size[2]};
    auto actor = createActor<GAnimActor>(position);
    actor->construct(GSource::getSource().getObject(texId), beginIndex,
                     endIndex, frameSpeed);
    actor->sprComp->setSizeWs(sizeWs);
  }
  // landscape
  const std::string shaderInstPath =
      jsobj["landScapeShader"].get<std::string>();
  landScape.init(gridMap.beginPoint.x, gridMap.beginPoint.y,
                 gridMap.width * gridMap.column, gridMap.height * gridMap.row,
                 "res/shaderInst.json");
  // gameMode

  setGameMode(jsobj["gameMode"]["playerClass"]);
  std::vector<float> playerPos =
      jsobj["gameMode"]["playerPosition"].get<std::vector<float>>();
  gm.player->setPositionWs({playerPos[0], playerPos[1], playerPos[2]});
  ////////////////////////////////////////////////////////////////////
  // 解除阻塞
  isDataLoadComplete = true;
}
void GWorld::pollActorsActive(GameWindow &window_) {
  int centerId =
      gridMap.getPositionIndex(window_.getCameraActve()->getPositionWs());
  gridMap.setActorsAlive(centerId);
  for (GActor *actor : gridMap.actorsAlive) {
    actor->loop(deltaTime, window_);
    actor->tick();
  }
  std::sort(allRenderObj.begin(), allRenderObj.end(),
            [](GRenderObjComponent *a, GRenderObjComponent *b) {
              return a->getPositionWs().y < b->getPositionWs().y;
            });
  window_.getCameraActve()->drawAllRenDerObj(allRenderObj, window_);
  allRenderObj.resize(0);
}
void GWorld::bindDefaultCameraController() {

  gm.gameIns->window.setCameraActive(&cameraDefault);
  controllerActive = &controllerDefault;
  controllerDefault.bind(GController::w, [&]() {
    cameraDefault.setPositionWs(cameraDefault.getPositionWs() +
                                FVector3(0, -0.1, 0));
  });
  controllerDefault.bind(GController::s, [&]() {
    cameraDefault.setPositionWs(cameraDefault.getPositionWs() +
                                FVector3(0, 0.1, 0));
  });
  controllerDefault.bind(GController::a, [&]() {
    cameraDefault.setPositionWs(cameraDefault.getPositionWs() +
                                FVector3(-0.1, 0, 0));
  });
  controllerDefault.bind(GController::d, [&]() {
    cameraDefault.setPositionWs(cameraDefault.getPositionWs() +
                                FVector3(0.1, 0, 0));
  });
  controllerDefault.bind(GController::kup, [&]() {
    float x = cameraDefault.getPixSize() - 0.0005;
    if (x > 0) {
      cameraDefault.setPixSize(x);
    }
  });
  controllerDefault.bind(GController::kdown, [&]() {
    cameraDefault.setPixSize(cameraDefault.getPixSize() + 0.0005);
  });
}
void GWorld::loop(GameWindow &window_, EventBase &event_) {
  deltaTime = clock.restart().asSeconds();

  controllerActive->loop(window_, event_);

  //  计时器任务
  timeManager.loop();
  // 渲染
  window_.clear(ColorBase::Black);
  // 渲染地图
  landScape.draw(window_);
  // actor逻辑
  pollActorsActive(window_);
  // 世界tick
  tick();
  // UI逻辑
  //
  // debug
  showGridMap(window_);
  GDebug::debugDisplay(window_);
  // test
  PRINTDEBUG(L"Actors:%d", gridMap.getActorsNumber());
  window_.display();
}
void GCameraObj::drawSpr(GRenderObjComponent *spr_, GameWindow &window_) {
  int winW = window_.getDefaultView().getSize().x;
  int winH = window_.getDefaultView().getSize().y;
  const IVector2 &posWin = wsToWin(spr_->getPositionWs(), winW, winH);
  spr_->getRenderSpr()->setPositionWin(posWin.x, posWin.y);
  const FVector3 &sizeWin = spr_->getSizeWs() / pixSize;
  spr_->getRenderSpr()->setSizeWin(sizeWin.x, sizeWin.y);
  spr_->getRenderSpr()->drawWin(window_);
}
void GCameraObj::drawAllRenDerObj(std::vector<GRenderObjComponent *> rObjs,
                                  GameWindow &window_) {
  renderFix();
  for (GRenderObjComponent *rObj : rObjs) {
    drawSpr(rObj, window_);
  }
}
void GWorld::tick() {

};
void GWorld::showGridMap(GameWindow &window_) {
#ifdef EDITOR
  FVector2 p = gridMap
                   .allNode[gridMap.getPositionIndex(
                       gm.gameIns->window.getCameraActve()->getPositionWs())]
                   .point;
  FVector3 p1 = {p.x, p.y, 0};
  FVector3 p2 = {p.x + gridmapNode<GActor>::gridmapNodeWidth, p.y, 0};
  FVector3 p3 = {p.x + gridmapNode<GActor>::gridmapNodeWidth,
                 p.y + gridmapNode<GActor>::gridmapNodeHeight, 0};
  FVector3 p4 = {p.x, p.y + gridmapNode<GActor>::gridmapNodeHeight, 0};
  std::vector<FVector3> points;
  points.push_back(p1);
  points.push_back(p2);
  points.push_back(p3);
  points.push_back(p4);
  points.push_back(p1);
  window_.getCameraActve()->drawLineWs(points, window_);
#endif
}

void GWorld::setCameraActive(GCameraObj *camera_) {
  gm.gameIns->window.setCameraActive(camera_);
}
GCameraObj *GWorld::getCameraActive() {
  return gm.gameIns->window.getCameraActve();
}
GActor::GActor() {
  worldPtr = GWorld::actorContext.______worldParamForCreate;
  nodeId = 0;
};