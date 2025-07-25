#include <framework.h>

int main() {
    GGame *gameins = GGame::getGameIns();
    gameins->loop();
    return 0;
}
void GActor::loop(float deltatime, WindowBase &window_) {
    for (GComponent *comp : __allComponents) {
        comp->loop(window_);
    }
}
void GActor::setActive() { isActive = true; };
void GActor::disableActive() { isActive = false; };

FVector3 GSceneComponent::getPositionWs() {
    if (parentComp)
        return parentComp->getPositionWs() + positionRelative;
    return owner->getPositionWs() + positionRelative;
}
void GStaticSpriteComponent::loop(WindowBase &window_) { owner->getWorld()->getCameraActive()->drawSpr(&spr, window_, getPositionWs()); }
GPlayer::GPlayer() {
    moveComp = createComponent<GMoveComponent>();
    camera = createComponent<GCameraComponent>();
    spr = createComponent<GStaticSpriteComponent>();
    tex.init(1, 1, 0.5, 1, "res/a.png");
    spr->setTex(tex);
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
        FVector3 target = camera->winToWs(sf::Mouse::getPosition(GGame::getGameIns()->window), GGame::getGameIns()->window);
        moveComp->setTarget(target);
    });
}
