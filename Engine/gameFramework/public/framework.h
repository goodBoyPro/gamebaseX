#ifndef FRAMEWORK_H
#define FRAMEWORK_H
#include "heads.h"
#include <timeManager.h>
#include "controller.h"
class GObject {
  public:
    ~GObject() {};
};
class GComponent : public GObject {
  public:
    virtual void loop() {};
};
class GRenderObj : public GComponent {};
class GActor : GObject {
  private:
    static std::vector<GActor *> allActorsActive;

  private:
    std::vector<GComponent *> __allComponents;
    std::vector<class GActorComponent *> __allActorComponents;
    FVector3 positionWs = {0, 0, 0};

  public:
    virtual void draw(class GCamera *camera_, WindowBase &window_,int winW_,int winH_);
    virtual void loop(float deltatime, WindowBase &window);
    const FVector3 &getPositionWs() const { return positionWs; }
    void setPositionWs(const FVector3 &posWs_) { positionWs = posWs_; };
    GComponent *createComponent();
    GActorComponent *createActorComponent();
    bool isActive = false;
    void setActive();
    void disableActive();

  public:
    static void loopAllActorsActive(float deltatime_, class GCamera *camera_, WindowBase &window_) {
        int winW = window_.getSize().x;
        int winH=window_.getSize().y;
        for (GActor *actor : allActorsActive) {
            actor->loop(deltatime_, window_);
            actor->draw(camera_,window_,winW,winH);
        }
    }
};
inline std::vector<GActor *> GActor::allActorsActive;
class GActorComponent : public GActor {
  private:
    GActor *parent = nullptr;
    FVector3 positionRelative = {0, 0, 0};
    void followParent() { setPositionWs(parent->getPositionWs() + positionRelative); }

  public:
    GActorComponent() {}
    const FVector3 &getPositionRelative() const { return positionRelative; }
    void setPositionRelative(const FVector3 &posRelative) { positionRelative = posRelative; }
    virtual void loop(float deltatime, WindowBase &window) override { followParent(); }
};

class GCamera : public GActorComponent {
  private:
    FVector3 positionForRender;

  public:
    // 统一渲染时相机位置
    void renderFix() { positionForRender = getPositionWs(); }
    
};
class GPlayer : public GActor {
  public:
};
class GWorld : public GObject {
  private:
    struct ActorsType {};
    std::vector<GActor *> allActors;
    GCamera *cameraActive = nullptr;
    TimeManager timeManager;
    GController controller;

  public:
    GWorld() {}
    void loop(float deltaTime_, WindowBase &window_, EventBase &event_) {
        controller.loop(window_, event_);

        //  计时器任务
        timeManager.loop();
        // cameraActive->renderFix();
        // 渲染
        window_.clear(sf::Color::Black);
        // actor逻辑
        GActor::loopAllActorsActive(deltaTime_,cameraActive, window_);
        window_.display();
    }
};
class GGame : GObject {
  private:
    GWorld *curWorld = nullptr;
    WindowBase window;
    sf::Event event;

  public:
    GGame() {
        window.create(sf::VideoMode(800, 600), "Game");
        curWorld = new GWorld();
    }
    void loop() {
        float deltaTime = 0.1;

        while (window.isOpen()) {

            curWorld->loop(deltaTime, window, event);
        }
    }
};

int main();
#endif // FRAMEWORK_H