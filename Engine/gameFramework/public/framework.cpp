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
  if (owner) {
    return owner->getPositionWs() + positionRelative;
  }
  return {0, 0, 0};
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
  controller.bindLinearInput(GController::a, [this]() {
    moveComp->moveX = -1;
    moveComp->isAutoMove = false;
  });
  controller.bindLinearInput(GController::d, [this]() {
    moveComp->moveX = 1;
    moveComp->isAutoMove = false;
  });
  controller.bindLinearInput(GController::w, [this]() {
    moveComp->moveY = -1;
    moveComp->isAutoMove = false;
  });
  controller.bindLinearInput(GController::s, [this]() {
    moveComp->moveY = 1;
    moveComp->isAutoMove = false;
  });
  controller.bindClickInput(GController::mleft, [this]() {
    FVector3 target =
        cameraComp->camera.getMousePositionWs(getWorld()->gm.gameIns->window);
    moveComp->setTarget(target);
  });
  controller.bindLinearInput(GController::kup, [this]() {
    if (cameraComp->camera.getPixSize() < 0.0001)
      return;
    cameraComp->camera.setPixSize(cameraComp->camera.getPixSize() - 0.0001);
  });
  controller.bindLinearInput(GController::kdown, [this]() {
    cameraComp->camera.setPixSize(cameraComp->camera.getPixSize() + 0.0001);
  });
}

void GPlayer::beginPlay() {
  sprComp->setTex(*(GTextureTree::getSource().getObject("res/base/player.png").getPtr()));
  sprComp->getSprite().setId(0);
  sprComp->getSprite().setCenter(0.5, 1);
  sprComp->setSizeWs({1, 1, 0});
}
///////////////////////////////////////////////////////////////////////////////////////////
void GWorld::loadBaseActors(const std::string &jsonPath_) {
  std::thread th([this, jsonPath_]() { asyncLoad(jsonPath_); });
  th.detach();
}
GStaticActor *GWorld::createStaticActor(const std::string &name_,
                                        const FVector3 &pos_,
                                        const FVector3 &sizeWs_) {
  auto actor = createActor<GStaticActor>(pos_);

  StaticActorInfo &staticActorInfo = ClassInfo::getStaticActorInfo(name_);
  actor->infoPtr = &staticActorInfo;
  actor->construct(*(GTextureTree::getSource().getObject(staticActorInfo.texPath).getPtr()),
                   staticActorInfo.texIndex);
  actor->sprComp->setSizeWs(sizeWs_);
  return actor;
}
void GWorld::asyncLoad(std::string jsonPath_) {
  if (jsonPath_ == "") {
    gridMap.init(2, 2, 300, 300);
    isDataLoadComplete = true;
    return;
  }
  nlohmann::json jsobj;
  std::ifstream ifile(jsonPath_);
  ifile >> jsobj;
  ifile.close();
  // 初始化地图
  gridMap.init(jsobj["mapInfo"]["row"], jsobj["mapInfo"]["column"],
               jsobj["mapInfo"]["width"], jsobj["mapInfo"]["height"]);

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
    actor->construct(*(GTextureTree::getSource().getObject(texId).getPtr()), beginIndex,
                     endIndex, frameSpeed);
    actor->sprComp->setSizeWs(sizeWs);
  }
  // landscape
  const std::string shaderInstPath =
      jsobj["landScapeShader"].get<std::string>();
  landScape.init(gridMap.beginPoint.x, gridMap.beginPoint.y,
                 gridMap.width * gridMap.column, gridMap.height * gridMap.row,
                 shaderInstPath);
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
  if (centerId == -1)
    return;
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
  useDefaultControllerAndCamera();
  controllerDefault.bindLinearInput(GController::w, [&]() {
    cameraDefault.setPositionWs(cameraDefault.getPositionWs() +
                                FVector3(0, -0.1, 0));
  });
  controllerDefault.bindLinearInput(GController::s, [&]() {
    cameraDefault.setPositionWs(cameraDefault.getPositionWs() +
                                FVector3(0, 0.1, 0));
  });
  controllerDefault.bindLinearInput(GController::a, [&]() {
    cameraDefault.setPositionWs(cameraDefault.getPositionWs() +
                                FVector3(-0.1, 0, 0));
  });
  controllerDefault.bindLinearInput(GController::d, [&]() {
    cameraDefault.setPositionWs(cameraDefault.getPositionWs() +
                                FVector3(0.1, 0, 0));
  });
  controllerDefault.bindLinearInput(GController::kup, [&]() {
    float x = cameraDefault.getPixSize() - 0.0005;
    if (x > 0) {
      cameraDefault.setPixSize(x);
    }
  });
  controllerDefault.bindLinearInput(GController::kdown, [&]() {
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
  render(window_, event_);
  GDebug::debugDisplay(window_);
  window_.display();
}
void GWorld::render(GameWindow &window_, EventBase &event_) {

  // 渲染地图
  landScape.draw(window_);
  // actor逻辑
  pollActorsActive(window_);
  // 世界tick
  tick();
  // UI逻辑
  //
  showGridMap(window_);

  // test
  PRINTDEBUG(L"Actors:%d", gridMap.getActorsNumber());
};
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
  int id = gridMap.getPositionIndex(
      gm.gameIns->window.getCameraActve()->getPositionWs());
  if (id == -1)
    return;
  FVector2 p = gridMap.allNode[id].point;
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
void GWorld::useDefaultControllerAndCamera() {
  gm.gameIns->window.setCameraActive(&cameraDefault);
  controllerActive = &controllerDefault;
}
void PageGameWaitSourceLoad::loop(GameWindow &window_, EventBase &event_) {
  while (window_.pollEvent(event_)) {
    if (event_.type == sf::Event::Closed) {
      window_.close();
    }
  }
  window_.clear();
  printText(window_, L"加载中");
  window_.display();
  if (gm.gameIns->worldLoading && gm.gameIns->worldLoading->isLoadComplete()) {
    gm.gameIns->curWorld = gm.gameIns->worldLoading;
    doSomethingBoforeToWorld();
  }
}
GObject *GObject::constructObject(const std::string &className_) {
  if (GClass::getClassRegInfo().find(className_) ==
      GClass::getClassRegInfo().end()) {
    throw std::overflow_error(className_ +
                              ":constructObject use unRegistered className\n");
  }
  return GClass::getClassRegInfo()[className_].constructCbk();
}
void GSceneComponent::setPositionRelative(const FVector3 &posRelative_) {
  positionRelative = posRelative_;
}
void GSceneComponent::attachToComponent(GSceneComponent *parent_) {
  parentComp = parent_;
}
FVector3 &GSceneComponent::getPositionRelative() { return positionRelative; }
FVector3 GSceneComponent::getSizeWs() { return sizeWs; }
void GSceneComponent::setSizeWs(const FVector3 &sizeWs_) { sizeWs = sizeWs_; }
void GRenderObjComponent::setRenderSpr(GSprite *spr_) { sprite = spr_; }
GSprite *GRenderObjComponent::getRenderSpr() { return sprite; }
void GRenderObjComponent::setSizeWin(float width_, float height_) {
  sprite->setSizeWin(width_, height_);
}
GStaticSpriteComponent::GStaticSpriteComponent() { setRenderSpr(&spr); }
void GStaticSpriteComponent::setTex(GTexture &tex) { spr.init(tex); }
GSprite &GStaticSpriteComponent::getSprite() { return spr; }
GStaticSpriteComponent::GStaticSpriteComponent(GTexture &tex) { setTex(tex); }
GSympleAnimationComponent::GSympleAnimationComponent() {
  setRenderSpr(&anim);
  anim.play();
}
GAnimation &GSympleAnimationComponent::getAnimation() { return anim; }
void GSympleAnimationComponent::setAnim(GTexture &tex, int beg_, int end_) {
  anim.init(tex, beg_, end_);
}
void GSympleAnimationComponent::loop(GameWindow &window_, float deltaTime_) {
  GRenderObjComponent::loop(window_, deltaTime_);
  anim.loop(deltaTime_);
}
void GActor::actorBaseInit(GWorld *worldPtr_) { worldPtr = worldPtr_; }
GWorld *GActor::getWorld() { return worldPtr; }
const FVector3 &GActor::getPositionWs() const { return positionWs; }
void GActor::addPositionOffsetWs(const FVector3 &offset) {
  setPositionWs(positionWs + offset);
}
void GActor::setRootComponent(GSceneComponent *comp_) {
  rootComponent = comp_;
  comp_->construct(this);
}
void GActor::destroyActor() { isValid = false; };
GActor::~GActor() {
  for (GComponent *comp : __allComponents) {
    delete comp;
  }
};
void GActor::loopAllActorsActive(std::vector<GActor *> allActorsActive_,
                                 float deltatime_, class GCameraObj *camera_,
                                 GameWindow &window_) {
  for (GActor *actor : allActorsActive_) {
    actor->loop(deltatime_, window_);
  }
}
void GCameraObj::renderFix() { positionForRender = getPositionWs(); }
FVector3 GCameraObj::getPositionWs() {
  if (parentComp) {
    return parentComp->getPositionWs();
  }
  return positionWs;
}
void GCameraObj::setPositionWs(const FVector3 &posWs_) {
  if (parentComp) {
    return;
  }
  positionWs = posWs_;
}
IVector2 GCameraObj::wsToWin(const FVector3 &PositionInWS, float winW_,
                             float win_H) {
  return {(int)(((PositionInWS.x - getPositionWs().x) / pixSize + winW_ / 2.f)),
          (int)(((PositionInWS.y - getPositionWs().y) / pixSize + win_H / 2.f -
                 (PositionInWS.z / pixSize)))};
}
void GCameraObj::setWindow(GameWindow *window_) { window = window_; }
GameWindow *GCameraObj::getWindow() { return window; }
float GCameraObj::getPixSize() { return pixSize; }
void GCameraObj::setPixSize(float pSize_) { pixSize = pSize_; }
FVector3 GCameraObj::winToWs(const FVector2 &posWin_, GameWindow &window_) {
  return {(posWin_.x - window_.getDefaultView().getSize().x / 2) * pixSize +
              getPositionWs().x,
          (posWin_.y - window_.getDefaultView().getSize().y / 2) * pixSize +
              getPositionWs().y,
          0};
}
FVector3 GCameraObj::getMousePositionWs(GameWindow &window_) {
  const IVector2 &posWin = sf::Mouse::getPosition(window_);
  FVector2 posfix = {
      (posWin.x * window_.getDefaultView().getSize().x / window_.getSize().x),
      (posWin.y * window_.getDefaultView().getSize().y / window_.getSize().y)};

  return winToWs(posfix, window_);
};
void GCameraObj::drawLineWin(const std::vector<FVector2> &points_,
                             GameWindow &window_, ColorBase color) {
  static std::vector<sf::Vertex> points;
  points.resize(0);
  int n = 0;
  for (const FVector2 &pos : points_) {
    points.emplace_back(sf::Vertex({(float)pos.x, (float)pos.y}, color));
    if (n)
      points.emplace_back(sf::Vertex({(float)pos.x, (float)pos.y}, color));
    n++;
  }
  window_.draw(points.data(), points.size(), sf::Lines);
}
void GCameraObj::drawLineWs(const std::vector<FVector3> &pointsVector_,
                            GameWindow &window_, ColorBase color) {
  points.resize(0);
  int n = 0;
  int winW = window_.getDefaultView().getSize().x;
  int winH = window_.getDefaultView().getSize().y;
  for (const FVector3 &pos : pointsVector_) {
    const IVector2 &poswin = wsToWin(pos, winW, winH);
    points.emplace_back(sf::Vertex({(float)poswin.x, (float)poswin.y}, color));
    if (n)
      points.emplace_back(
          sf::Vertex({(float)poswin.x, (float)poswin.y}, color));
    n++;
  }

  window_.draw(points.data(), points.size(), sf::Lines);
}
GCameraComponent::GCameraComponent() { camera.parentComp = this; }
FVector3 GMoveComponent::getMoveVector() { return moveVector; }
void GMoveComponent::move(float deltaTime_) {
  moveVector = vectorNormalize({moveX, moveY, moveZ});
  owner->addPositionOffsetWs(moveVector * speed * deltaTime_);
  moveX = 0;
  moveY = 0;
  moveZ = 0;
}
void GMoveComponent::autoMove(float deltaTime_) {
  const FVector3 &vec = target - owner->getPositionWs();
  float len = getVectorLen(vec);
  if (len < 0.01) {
    moveVector = {0, 0, 0};
    isAutoMove = false;
  } else {
    moveVector = vectorNormalize(vec);
  }
  owner->addPositionOffsetWs(moveVector * speed * deltaTime_);
}
void GMoveComponent::loop(GameWindow &window_, float deltaTime_) {
  if (isAutoMove) {
    autoMove(deltaTime_);
  } else {
    move(deltaTime_);
  }
}
void GAnimActor::construct(GTexture &tex_, int begin_, int end_,
                           int frameSpeed_) {
  sprComp = createComponent<GSympleAnimationComponent>();
  sprComp->setAnim(tex_, begin_, end_);
  sprComp->getAnimation().setFramePerS(frameSpeed_);
}
void GStaticActor::construct(GTexture &tex_, int id_) {
  sprComp = createComponent<GStaticSpriteComponent>();
  sprComp->getSprite().init(tex_);
  sprComp->getSprite().setId(id_);
}
GMaterial &GLandScape::getMaterial() { return landMaterial; }
void GLandScape::init(float beginX_, float beginY_, float widthTotal_,
                      float heightTotal_, const std::string &matPath_) {
  beginX = beginX_;
  beginY = beginY_;
  widhtTotal = widthTotal_;
  heightTotal = heightTotal_;
  spr.init(*(GTextureTree::getSource().getObject("res/base/texture/pix1.png").getPtr()));
  if (matPath_ == "") {
  } else {
    landMaterial.init(matPath_);
  }
}
void GLandScape::draw(GameWindow &window_) {
  float pixSize = window_.getCameraActve()->getPixSize();
  const IVector2 &winpos = window_.wsToWin({beginX, beginY, 0});
  spr.setPositionWin(winpos.x, winpos.y);
  spr.setSizeWin(widhtTotal / pixSize, heightTotal / pixSize);
  landMaterial.draw(spr, window_);
}
void GWorld::setControllerActive(GController *controller_) {
  controllerActive = controller_;
}
void GWorld::setActorContext() {
  actorContext.______worldParamForCreate = this;
}
GridMap<GActor> &GWorld::getGridMap() { return gridMap; }
GActor *GWorld::createActorByClassName(const std::string &className_,
                                       const FVector3 &position) {
  setActorContext();
  GActor *rtn = (GActor *)GObject::constructObject(className_);
  rtn->nodeId = gridMap.addActor(rtn);
  rtn->setPositionWs(position);
  rtn->beginPlay();
  return rtn;
}
std::vector<GRenderObjComponent *> &GWorld::getRenderObjComps() {
  return allRenderObj;
}
GController *GWorld::getControllerActive() { return controllerActive; }
TimeManager &GWorld::getTimeManager() { return timeManager; }
void GGame::setCameraActive(GCameraObj *camera_) {
  window.setCameraActive(camera_);
};
GGame::GGame() {
  curWorld = &waitPage;
  waitPage.gm.gameIns = this;

  window.create(
      sf::VideoMode(getGameConfig().windowWidth, getGameConfig().windowHeight),
      "Game");
  window.setFramerateLimit(getGameConfig().frameLimit);
  sf::Image icon;
  icon.loadFromFile(getGameConfig().windowIcon);
  window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
}
GGame::~GGame() {
  if (curWorld != &waitPage)
    delete curWorld;
}
void GGame::loop() {

  while (window.isOpen()) {

    curWorld->loop(window, event);
  }
}
