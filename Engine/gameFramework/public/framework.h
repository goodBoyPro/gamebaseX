#ifndef FRAMEWORK_H
#define FRAMEWORK_H
#include "heads.h"
#include <timeManager.h>
#include "controller.h"
#include "render/sprite.h"
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
    virtual void loop(float deltatime_, WindowBase &window_, class GCamera *camera_);
    const FVector3 &getPositionWs() const { return positionWs; }
    void setPositionWs(const FVector3 &posWs_) { positionWs = posWs_; };
    GComponent *createComponent();
    GActorComponent *createActorComponent();
    bool isActive = false;
    void setActive();
    void disableActive();

  public:
    static void loopAllActorsActive(float deltatime_, class GCamera *camera_, WindowBase &window_) {
        for (GActor *actor : allActorsActive) {
            actor->loop(deltatime_, window_, camera_);
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
    virtual void loop(float deltatime, WindowBase &window, GCamera *camera_) override { followParent(); }
};

class GCamera : public GActorComponent {
  private:
    FVector3 positionForRender;
    FVector3 wsToWin(const FVector3 &PositionInWS, float winW_, float win_H, float pixSize = 1) {
        return {((PositionInWS.x - getPositionWs().x) / pixSize + winW_ / 2.f), ((PositionInWS.y - getPositionWs().y) / pixSize + win_H / 2.f - (PositionInWS.z / pixSize)), 0};
    }

  public:
    // 统一渲染时相机位置
    void renderFix() { positionForRender = getPositionWs(); }
    void drawSpr(GSprite &spr_, WindowBase &window_, const FVector3 &posWs_) {
        int winW = window_.getSize().x;
        int winH = window_.getSize().y;
        const FVector3 &posWin = wsToWin(posWs_, winW, winH);
        spr_.setPositionWin(posWin.x, posWin.y);
        spr_.drawWin(window_);
    }
};
class GPlayer : public GActor {
  public:
};
class GActorStatic : public GActor {
    GSprite spr;

  public:
    GActorStatic(GTexture &tex, int row, int column) : spr(tex, row, column) {}
    void draw(WindowBase &window_, GCamera *camera_) { 
      spr.drawWin(window_);
      // camera_->drawSpr(spr, window_, getPositionWs());
       }
    void loop(float deltatime, WindowBase &window, GCamera *camera_) override {
        GActor::loop(deltatime, window, camera_);
        draw(window, camera_);
    }
};
class GWorld : public GObject {
  private:
    struct ActorsType {};
    std::vector<GActor *> allActors;
    GCamera cameraDefault;
    GCamera *cameraActive = nullptr;
    TimeManager timeManager;
    GController controllerDefault;
    GController *controllerActive = nullptr;
  GTexture tex;
  public:
    GWorld() {
        cameraActive = &cameraDefault;
        controllerActive = &controllerDefault;
        
        tex.loadFromFile("res/ui1.png");       
        GActorStatic *ac1=new GActorStatic(tex,1,1);
        ac1->setActive();
    }
    void loop(float deltaTime_, WindowBase &window_, EventBase &event_) {
        controllerActive->loop(window_, event_);
        
        //  计时器任务
        timeManager.loop();
        cameraActive->renderFix();
        // 渲染
        window_.clear(sf::Color::Black);
        
        // actor逻辑
        GActor::loopAllActorsActive(deltaTime_, cameraActive, window_);
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