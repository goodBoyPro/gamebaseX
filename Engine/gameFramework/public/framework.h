#ifndef FRAMEWORK_H
#define FRAMEWORK_H
#include "heads.h"
#include <timeManager.h>
#include "controller.h"
#include "render/sprite.h"
class GObject {
  public:
    virtual ~GObject() {};
};
class GComponent : public GObject {
    protected:
    class GActor *parent = nullptr;

  public:
    GComponent() {}
    void init(GActor *parent_) { parent = parent_; }
    virtual void loop() {};
    virtual void setActive() {}
    virtual void disableActive(){}
};
class GRenderObj : public GComponent {};
class GActor : GObject {
  private:
    static std::vector<GActor *> allActorsActive;

  private:
    std::vector<GComponent *> __allComponents;
    FVector3 positionWs = {0, 0, 0};
    class GWorld *worldPtr = nullptr;

  public:
    GWorld *getWorld() { return worldPtr; }
    virtual void loop(float deltatime_, WindowBase &window_, class GCamera *camera_);
    const FVector3 &getPositionWs() const { return positionWs; }
    void setPositionWs(const FVector3 &posWs_) { positionWs = posWs_; };
    template<class T>
    T *createComponent() {
        T *comp = new T();
        comp->init(this);
        __allComponents.push_back(comp);
        return comp;
    }
    bool isActive = false;
    void setActive();
    void disableActive();

  public:
  public:
    static void loopAllActorsActive(float deltatime_, class GCamera *camera_, WindowBase &window_) {
        for (GActor *actor : allActorsActive) {
            actor->loop(deltatime_, window_, camera_);
        }
    }
};
inline std::vector<GActor *> GActor::allActorsActive;
class GActorComponentX : public GActor {
  private:
    GActor *parent = nullptr;
    FVector3 positionRelative = {0, 0, 0};
    void followParent() { setPositionWs(parent->getPositionWs() + positionRelative); }

  public:
    GActorComponentX() {}
    const FVector3 &getPositionRelative() const { return positionRelative; }
    void setPositionRelative(const FVector3 &posRelative) { positionRelative = posRelative; }
    virtual void loop(float deltatime, WindowBase &window, GCamera *camera_) override { followParent(); }
};
class GActorComponentIF : public GComponent {
    GActor *actor = nullptr;
    FVector3 positionRelative = {0, 0, 0};
  public:
    GActorComponentIF() {}   
    void followParent() { actor->setPositionWs(parent->getPositionWs() + positionRelative); }
    const FVector3 &getPositionRelative() const { return positionRelative; }
    void setPositionRelative(const FVector3 &posRelative) { positionRelative = posRelative; }
    virtual void loop() override { followParent(); }
    void setActive() override {actor->setActive();}
    void disableActive() override{actor->disableActive();}
    
};

class GCamera : public GActorComponentX {
  private:
    float pixSize=15;
    FVector3 positionForRender;
    FVector3 wsToWin(const FVector3 &PositionInWS, float winW_, float win_H) {
        return {((PositionInWS.x - getPositionWs().x) / pixSize + winW_ / 2.f), ((PositionInWS.y - getPositionWs().y) / pixSize + win_H / 2.f - (PositionInWS.z / pixSize)), 0};
    }


  public:
    // 统一渲染时相机位置
    void renderFix() { positionForRender = getPositionWs(); }
    void drawSpr(GSprite *spr_, WindowBase &window_, const FVector3 &posWs_) {
        int winW = window_.getSize().x;
        int winH = window_.getSize().y;
        const FVector3 &posWin = wsToWin(posWs_, winW, winH);
        spr_->setPositionWin(posWin.x, posWin.y);
        spr_->drawWin(window_);
    }
};
class GPlayer : public GActor {
  public:
};
class GActorIF : public GActor {
    GSprite *spr = nullptr;

  public:
    GActorIF() {}
    void draw(WindowBase &window_, GCamera *camera_) { camera_->drawSpr(spr, window_, getPositionWs()); }
    void loop(float deltatime, WindowBase &window, GCamera *camera_) override {
        GActor::loop(deltatime, window, camera_);
        draw(window, camera_);
    }
    void setActiveSpr(GSprite *sprPtr_) { spr = sprPtr_; }
};
class GActorStatic : public GActorIF {
  private:
    GSprite spr;

  public:
    GActorStatic() {}
    void init(GTexture &tex_) {
        spr.init(tex_);
        setActiveSpr(&spr);
    }
    GActorStatic(GTexture &tex_) { init(tex_); }
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
    void bindDefaultCameraController() {
        cameraActive = &cameraDefault;
        controllerActive = &controllerDefault;
        controllerDefault.bind(GController::w, [&]() { cameraDefault.setPositionWs(cameraDefault.getPositionWs() + FVector3(0, -0.1, 0)); });
        controllerDefault.bind(GController::s, [&]() { cameraDefault.setPositionWs(cameraDefault.getPositionWs() + FVector3(0, 0.1, 0)); });
        controllerDefault.bind(GController::a, [&]() { cameraDefault.setPositionWs(cameraDefault.getPositionWs() + FVector3(-0.1, 0, 0)); });
        controllerDefault.bind(GController::d, [&]() { cameraDefault.setPositionWs(cameraDefault.getPositionWs() + FVector3(0.1, 0, 0)); });
    }

  public:
    GWorld() {
        bindDefaultCameraController();

        // test
        tex.init(1, 1, "res/ui1.png");
        GActorStatic *ac1 = new GActorStatic(tex);
        ac1->setActive();
        //
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