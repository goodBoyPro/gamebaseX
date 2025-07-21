#include <framework.h>

int main() {
    GGame gameins;
    gameins.loop();
    return 0;
}
void GActor::loop(float deltatime, WindowBase &window) {
    for (GComponent *comp : __allComponents) {
        comp->loop();
    }
}
void GActor::setActive() {
    isActive = true;
    for (GActor *actor : __allActorComponents) {
        actor->setActive();
    }
    allActorsActive.push_back(this);
};
void GActor::disableActive() {
    isActive = false;
    for (GActor *actor : __allActorComponents) {
        actor->disableActive();
    }
};
void GActor::draw(class GCamera *camera_, WindowBase &window_,int winW_,int winH_) {

}
