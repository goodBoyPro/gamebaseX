#include <framework.h>

void GActor::loop(float deltatime, WindowBase &window_) {
    for (GComponent *comp : __allComponents) {
        comp->loop(window_, deltatime);
    }
}
void GActor::setActive() { isActive = true; };
void GActor::disableActive() { isActive = false; };

FVector3 GSceneComponent::getPositionWs() {
    if (parentComp)
        return parentComp->getPositionWs() + positionRelative;
    return owner->getPositionWs() + positionRelative;
}
void GRenderObjComponent::loop(WindowBase &window_, float deltaTime_) {
    // owner->getWorld()->getCameraActive()->drawSpr(sprite, window_, getPositionWs());
    owner->getWorld()->getRenderObjComps().push_back(this);
    sprite->posWs = getPositionWs();
}
GSource gs;
GPlayer::GPlayer() {
    moveComp = createComponent<GMoveComponent>();
    moveComp->speed = 100;
    cameraComp = createComponent<GCameraComponent>();
    sprComp = createComponent<GStaticSpriteComponent>();
   
    // tex.init(5, 5, 0.5, 1, "res/arr_110110_c_5_5_0_0_tree.png");
    // sprComp->setTex(tex);
    // sprComp->getSprite().setId(5);
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
        FVector3 target = cameraComp->winToWs(sf::Mouse::getPosition(GGame::getGameIns()->window), GGame::getGameIns()->window);
        moveComp->setTarget(target);
    });
}
void GActor::setPositionWs(const FVector3 &posWs_) {
  int nodeIdTemp = getWorld()->gridMap.getPositionIndex(posWs_);
  if (nodeIdTemp != nodeId) {
    getWorld()->gridMap.changeActorNode(this, nodeIdTemp, nodeId);
    nodeId=nodeIdTemp;
  }
  
  positionWs = posWs_;
};
void GPlayer::beginPlay() {
  sprComp->setTex(getWorld()->getSource()->getTexture(110110));
  sprComp->getSprite().setId(10);
}
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
  for (GRenderObjComponent *robj : allRenderObj) {
    cameraActive->drawSpr(robj, window_);
  }
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
