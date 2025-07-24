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
    class GActor *owner = nullptr;

  private:
  public:
    GComponent() {}
    virtual void init(GActor *owner_) { owner = owner_; }

    virtual void loop(WindowBase &window_) {}
};
class GSceneComponent : public GComponent {
  private:
    std::vector<GSceneComponent *> __childs;
    FVector3 positionRelative = {0, 0, 0};
    GSceneComponent *parentComp = nullptr;

  public:
    void setPositionRelative(const FVector3 &posRelative_) { positionRelative = posRelative_; }
    void attachToComponent(GSceneComponent *parent_) { parentComp = this; }
    FVector3 &getPositionRelative() { return positionRelative; }
    FVector3 getPositionWs();
};
class GPriimitiveComponent : public GSceneComponent {};
class GStaticSpriteComponent : public GSceneComponent {
    GSprite spr;

  public:
    GStaticSpriteComponent() {}
    void setTex(GTexture &tex) { spr.init(tex); }
    GStaticSpriteComponent(GTexture &tex) { setTex(tex); }
    virtual void loop(WindowBase &window_) override;
};
class GActor : GObject {
  private:
  private:
    std::vector<GComponent *> __allComponents;
    GSceneComponent *rootComponent = nullptr;
    FVector3 positionWs = {0, 0, 0};
    class GWorld *worldPtr = nullptr;

  public:
    GActor() {};
    void actorBaseInit(GWorld *worldPtr_) { worldPtr = worldPtr_; }
    GWorld *getWorld() { return worldPtr; }
    virtual void loop(float deltatime_, WindowBase &window_);
    const FVector3 &getPositionWs() const { return positionWs; }
    void setPositionWs(const FVector3 &posWs_) { positionWs = posWs_; };
    template <class T> T *createComponent() {
        T *comp = new T();
        comp->init(this);
        __allComponents.push_back(comp);
        return comp;
    }
    void setRootComponent(GSceneComponent *comp_) {
        rootComponent = comp_;
        comp_->init(this);
    }
    bool isActive = false;
    void setActive();
    void disableActive();
    ~GActor() {
        for (GComponent *comp : __allComponents) {
            delete comp;
        }
    };

  public:
  public:
    static void loopAllActorsActive(std::vector<GActor *> allActorsActive_, float deltatime_, class GCameraComponent *camera_, WindowBase &window_) {
        for (GActor *actor : allActorsActive_) {
            actor->loop(deltatime_, window_);
        }
    }
};

class GCameraComponent : public GSceneComponent {
  private:
    float pixSize = 15;
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
class GCamera : public GActor {

  public:
    GCameraComponent *cameraComp = nullptr;
    GCamera() { cameraComp = createComponent<GCameraComponent>(); }
};
class GPlayer : public GActor {
  public:
    GCameraComponent *camera = nullptr;
    GStaticSpriteComponent *spr = nullptr;
    GController controller;
    GTexture tex;
    GPlayer() {
        camera = createComponent<GCameraComponent>();
        spr = createComponent<GStaticSpriteComponent>();
        tex.init(1, 1, "res/a.png");
        spr->setTex(tex);
        controller.bind(GController::a, [this]() { setPositionWs(getPositionWs() + FVector3(-0.1, 0, 0)); });
        controller.bind(GController::d, [this]() { setPositionWs(getPositionWs() + FVector3(0.1, 0, 0)); });
        controller.bind(GController::w, [this]() { setPositionWs(getPositionWs() + FVector3(0, -0.1, 0)); });
        controller.bind(GController::s, [this]() { setPositionWs(getPositionWs() + FVector3(0, 0.1, 0)); });
    }
};
class actorTest : public GActor {
  public:
    GTexture tex_;
    actorTest() {
        tex_.init(1, 1, "res/test.png");
        GStaticSpriteComponent *comp = createComponent<GStaticSpriteComponent>();
        comp->setTex(tex_);
    }
};
class GameMode : public GObject {
  public:
    GPlayer *player = nullptr;
    GController *controller = nullptr;
    ~GameMode() {

        if (controller)
            delete controller;
    }
};

class GWorld : public GObject {
  private:
    GameMode gm;
    struct ActorsType {};
    std::vector<GActor *> allActorsActive;
    GCamera cameraDefault;
    GCameraComponent *cameraActive = nullptr;
    TimeManager timeManager;
    GController controllerDefault;
    GController *controllerActive = nullptr;
    GTexture tex;
    void bindDefaultCameraController() {
        cameraActive = (cameraDefault.cameraComp);
        controllerActive = &controllerDefault;
        controllerDefault.bind(GController::w, [&]() { cameraDefault.setPositionWs(cameraDefault.getPositionWs() + FVector3(0, -0.1, 0)); });
        controllerDefault.bind(GController::s, [&]() { cameraDefault.setPositionWs(cameraDefault.getPositionWs() + FVector3(0, 0.1, 0)); });
        controllerDefault.bind(GController::a, [&]() { cameraDefault.setPositionWs(cameraDefault.getPositionWs() + FVector3(-0.1, 0, 0)); });
        controllerDefault.bind(GController::d, [&]() { cameraDefault.setPositionWs(cameraDefault.getPositionWs() + FVector3(0.1, 0, 0)); });
    }

  public:
    template <class playerClass> void setGameMode() { gm.player = createActor<playerClass>(); }
    template <class T> T *createActor() {
        T *actor = new T();
        actor->actorBaseInit(this);
        allActorsActive.push_back(actor);
        return actor;
    }
    GCameraComponent *getCameraActive() { return cameraActive; }
    GController *getControllerActive() { return controllerActive; }

    GWorld() {
        bindDefaultCameraController();

        setGameMode<GPlayer>();
        if (gm.player) {
            cameraActive = (gm.player->camera);
            controllerActive = &(gm.player->controller);
        }
        createActor<actorTest>();
    }
    void loop(float deltaTime_, WindowBase &window_, EventBase &event_) {
        controllerActive->loop(window_, event_);

        //  计时器任务
        timeManager.loop();
        cameraActive->renderFix();
        // 渲染
        window_.clear(sf::Color::Black);

        // actor逻辑
        GActor::loopAllActorsActive(allActorsActive, deltaTime_, cameraActive, window_);
        window_.display();
    }
    ~GWorld() {}
};
class GGame : GObject {
  private:
    GWorld *curWorld = nullptr;
    WindowBase window;
    sf::Event event;

  public:
    GGame() {
        window.create(sf::VideoMode(800, 600), "Game");
        sf::Image icon;
        icon.loadFromFile("res/a.png");

        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
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