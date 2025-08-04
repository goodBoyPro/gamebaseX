#include <framework.h>

void GActor::loop(float deltatime, WindowBase &window_) {
  for (GComponent *comp : __allComponents) {
    comp->loop(window_, deltatime);
  }
}
void GActor::setActive() { isActive = true; };
void GActor::disableActive() { isActive = false; };
void GActor::setPositionWs(const FVector3 &posWs_) {
  int nodeIdTemp = getWorld()->gridMap.getPositionIndex(posWs_);
  if(!nodeIdTemp)return;
  if (nodeIdTemp != nodeId) {
    getWorld()->gridMap.changeActorNode(this, nodeIdTemp, nodeId);
    nodeId = nodeIdTemp;
  }

  positionWs = posWs_;
};
///////////////////////////////////////////////////////////////////////////////////////////
FVector3 GSceneComponent::getPositionWs() {
  if (parentComp)
    return parentComp->getPositionWs() + positionRelative;
  return owner->getPositionWs() + positionRelative;
}
///////////////////////////////////////////////////////////////////////////////////////////
void GRenderObjComponent::loop(WindowBase &window_, float deltaTime_) {
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
  controller.bind(GController::left, [this]() {
    FVector3 target =
        cameraComp->getMousePositionWs(GGame::getGameIns()->window);
    moveComp->setTarget(target);
  });
  controller.bind(GController::m, [this]() {
    cameraComp->setPixSize(cameraComp->getPixSize() + 0.001);
  });
}

void GPlayer::beginPlay() {
  sprComp->setTex(getWorld()->getSource()->getTexture(110110));
  sprComp->getSprite().setId(10);
  sprComp->setSizeWs({2, 2, 0});
}
///////////////////////////////////////////////////////////////////////////////////////////
void GWorld::loadBaseActors(const std::string &jsonPath_) {
  nlohmann::json jsobj;
  std::ifstream ifile(jsonPath_);
  ifile >> jsobj;
  ifile.close();
  // staticActor
  for (auto info : jsobj["staticActors"]) {
    int texId = info["texId"];
    int index = info["index"];
    const std::vector<float> &pos = info["position"].get<std::vector<float>>();
    FVector3 position = {pos[0], pos[1], pos[2]};
    const std::vector<float> &size = info["sizeWs"].get<std::vector<float>>();
    FVector3 sizeWs = {size[0], size[1], size[2]};
    auto actor = createActor<GStaticActor>(position);
    actor->construct(getSource()->getTexture(texId), index);
    actor->sprComp->setSizeWs(sizeWs);
  }
  // animActor
  for (auto info : jsobj["animActors"]) {
    int texId = info["texId"];
    int beginIndex = info["beginIndex"];
    int endIndex = info["endIndex"];
    int frameSpeed = info["frameSpeed"];
    const std::vector<float> &pos = info["position"].get<std::vector<float>>();
    FVector3 position = {pos[0], pos[1], pos[2]};
    const std::vector<float> &size = info["sizeWs"].get<std::vector<float>>();
    FVector3 sizeWs = {size[0], size[1], size[2]};
    auto actor = createActor<GAnimActor>(position);
    actor->construct(getSource()->getTexture(texId), beginIndex, endIndex,
                     frameSpeed);
    actor->sprComp->setSizeWs(sizeWs);
  }
}
void GWorld::pollActorsActive(WindowBase &window_) {
  int centerId = gridMap.getPositionIndex(cameraActive->getPositionWs());
  gridMap.setActorsAlive(centerId);
  for (GActor *actor : gridMap.actorsAlive) {
    actor->loop(deltaTime, window_);
    actor->tick();
  }
  std::sort(allRenderObj.begin(), allRenderObj.end(),
            [](GRenderObjComponent *a, GRenderObjComponent *b) {
              return a->getPositionWs().y < b->getPositionWs().y;
            });
  cameraActive->drawAllRenDerObj(allRenderObj, window_);
  allRenderObj.resize(0);
}
void GWorld::bindDefaultCameraController() {
  cameraActive = (cameraDefault.cameraComp);
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
}
void GWorld::loop(WindowBase &window_, EventBase &event_) {
  deltaTime = clock.restart().asSeconds();
  controllerActive->loop(window_, event_);

  //  计时器任务
  timeManager.loop();
  // 渲染
  window_.clear(sf::Color::Black);
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
void GCameraComponent::drawSpr(GRenderObjComponent *spr_, WindowBase &window_) {
  int winW = window_.getDefaultView().getSize().x;
  int winH = window_.getDefaultView().getSize().y;
  const FVector3 &posWin = wsToWin(spr_->getPositionWs(), winW, winH);
  spr_->getRenderSpr()->setPositionWin(posWin.x, posWin.y);
  const FVector3 &sizeWin = spr_->getSizeWs() / pixSize;
  spr_->getRenderSpr()->setSizeWin(sizeWin.x, sizeWin.y);
  spr_->getRenderSpr()->drawWin(window_);
}
void GCameraComponent::drawAllRenDerObj(
    std::vector<GRenderObjComponent *> rObjs, WindowBase &window_) {
  renderFix();
  for (GRenderObjComponent *rObj : rObjs) {
    drawSpr(rObj, window_);
  }
}
void GWorld::tick() {

};
void GWorld::showGridMap(WindowBase &window_) {
  #ifdef EDITOR
  FVector2 p = gridMap.allNode[gm.player->nodeId].point;
  FVector3 p1={p.x,p.y,0};
  FVector3 p2={p.x+gridmapNode<GActor>::gridmapNodeWidth,p.y,0};
  FVector3 p3={p.x+gridmapNode<GActor>::gridmapNodeWidth,p.y+gridmapNode<GActor>::gridmapNodeHeight,0};
  FVector3 p4={p.x,p.y+gridmapNode<GActor>::gridmapNodeHeight,0};
  std::vector<FVector3> points;
  points.push_back(p1);
  points.push_back(p2);
  points.push_back(p3);
  points.push_back(p4);
  points.push_back(p1);
  cameraActive->drawLineWs(points, window_);
  #endif
}
