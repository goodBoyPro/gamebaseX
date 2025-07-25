#include <framework.h>

int main() {
    GGame *gameins = GGame::getGameIns();
    gameins->loop();
    return 0;
}
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
void GRnderObjComponent::loop(WindowBase &window_, float deltaTime_) {
    // owner->getWorld()->getCameraActive()->drawSpr(sprite, window_, getPositionWs());
    owner->getWorld()->getRenderObjComps().push_back(sprite);
    sprite->posWs = getPositionWs();
}
GPlayer::GPlayer() {
    moveComp = createComponent<GMoveComponent>();
    moveComp->speed = 100;
    cameraComp = createComponent<GCameraComponent>();
    sprComp = createComponent<GStaticSpriteComponent>();
    tex.init(5, 5, 0.5, 1, "res/trees_256x5.png");
    sprComp->setTex(tex);
    sprComp->getSprite().setId(10);
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
