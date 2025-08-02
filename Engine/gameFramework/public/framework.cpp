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
    owner->getWorld()->getRenderObjComps().push_back(sprite);
    sprite->posWs = getPositionWs();
}
GSource gs;
GPlayer::GPlayer() {
    moveComp = createComponent<GMoveComponent>();
    moveComp->speed = 100;
    cameraComp = createComponent<GCameraComponent>();
    sprComp = createComponent<GStaticSpriteComponent>();

    sprComp->setTex(getWorld()->getSource()->getTexture(110110));
    sprComp->getSprite().setId(10);
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
  
  positionWs = posWs_; };