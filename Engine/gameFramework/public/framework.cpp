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
    for (GComponent *comp : __allComponents) {
        comp->setActive();
    }
    allActorsActive.push_back(this);
};
void GActor::disableActive() {
    isActive = false;
    for (GComponent*comp : __allComponents) {
        comp->disableActive();
    }
};

void GActorIF::loop(float deltatime, WindowBase &window) {
    GActor::loop(deltatime, window);
    draw(window, getWorld()->getCameraActive());
}
