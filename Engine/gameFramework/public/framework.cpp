#include <framework.h>

int main() {
    GGame gameins;
    gameins.loop();
    return 0;
}
void GActor::loop(float deltatime, WindowBase &window_) {
    for (GComponent *comp : __allComponents) {
        comp->loop(window_);
    }
}
void GActor::setActive() {
    isActive = true;
    for (GComponent *comp : __allComponents) {
        comp->setActive();
    }
    allActorsActive.push_back(this);
};
void GActor::disableActive() {
    isActive = false;
    for (GComponent *comp : __allComponents) {
        comp->disableActive();
    }
};

FVector3 GSceneComponent::getPositionWs() {
    if (parentComp)
        return parentComp->getPositionWs() + positionRelative;
    return owner->getPositionWs()+positionRelative;
}
void GStaticSpriteComponent::loop(WindowBase &window_) {
    owner->getWorld()->getCameraActive()->drawSpr(&spr,window_,getPositionWs());
}
