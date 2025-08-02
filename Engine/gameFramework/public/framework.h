#ifndef FRAMEWORK_H
#define FRAMEWORK_H
#include "heads.h"
#include <timeManager.h>
#include "controllerX.h"
#include "render/sprite.h"
#include "GComman.h"
#include"gridWorld.h"
#include<GDebug.h>
class GObject {
public:
  bool isValid=true;
    virtual ~GObject() {};
};
class GComponent : public GObject {
  protected:
    class GActor *owner = nullptr;

  private:
  public:
    GComponent() {}
    virtual void init(GActor *owner_) { owner = owner_; }

    virtual void loop(WindowBase &window_, float deltaTime_) {}
};
class GSceneComponent : public GComponent {
  private:
    std::vector<GSceneComponent *> __childs;
    FVector3 positionRelative = {0, 0, 0};
    GSceneComponent *parentComp = nullptr;

  public:
    void setPositionRelative(const FVector3 &posRelative_) { positionRelative = posRelative_; }
    void attachToComponent(GSceneComponent *parent_) { parentComp = parent_; }
    FVector3 &getPositionRelative() { return positionRelative; }
    FVector3 getPositionWs();
};
class GPriimitiveComponent : public GSceneComponent {};
class GRenderObjComponent : public GSceneComponent {
  private:
    GSprite *sprite = nullptr;

  public:
    void setRenderSpr(GSprite *spr_) { sprite = spr_; }
    GSprite *getRenderSpr() { return sprite; }
    virtual void loop(WindowBase &window_, float deltaTime_) override;
};
class GStaticSpriteComponent : public GRenderObjComponent {
    GSprite spr;

  public:
    GStaticSpriteComponent() { setRenderSpr(&spr); }
    void setTex(GTexture &tex) { spr.init(tex); }
    GSprite &getSprite() { return spr; }
    GStaticSpriteComponent(GTexture &tex) { setTex(tex); }
};
class GSympleAnimationComponent : public GRenderObjComponent {
  public:
    GAnimation anim;
    GSympleAnimationComponent() {
        setRenderSpr(&anim);
        anim.play();
    }
    GAnimation &getAnimation() { return anim; }
    void setAnim(GTexture &tex, int beg_, int end_) { anim.init(tex, beg_, end_); }
    void loop(WindowBase &window_, float deltaTime_) override {
        GRenderObjComponent::loop(window_, deltaTime_);
        anim.loop(deltaTime_);
    }
};
class GActor : public GObject {
  private:
  private:
    std::vector<GComponent *> __allComponents;
    GSceneComponent *rootComponent = nullptr;
    FVector3 positionWs = {0, 0, 0};
    class GWorld *worldPtr = nullptr;

  public:
    virtual void beginPlay() {}
    virtual void tick() {}
    GActor() {};
    void actorBaseInit(GWorld *worldPtr_) { worldPtr = worldPtr_; }
    GWorld *getWorld() { return worldPtr; }
    virtual void loop(float deltatime_, WindowBase &window_);
    const FVector3 &getPositionWs() const { return positionWs; }
    void setPositionWs(const FVector3 &posWs_);
    void addPositionOffsetWs(const FVector3 &offset) { setPositionWs(positionWs + offset); }
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
    float pixSize = 1;
    FVector3 positionForRender;
    FVector3 wsToWin(const FVector3 &PositionInWS, float winW_, float win_H) {
        return {((PositionInWS.x - getPositionWs().x) / pixSize + winW_ / 2.f), ((PositionInWS.y - getPositionWs().y) / pixSize + win_H / 2.f - (PositionInWS.z / pixSize)), 0};
    }

  public:
    FVector3 winToWs(const IVector2 &posWin_, WindowBase &window_) {
        return {(posWin_.x - window_.getDefaultView().getSize().x / 2) * pixSize + getPositionWs().x, (posWin_.y - window_.getDefaultView().getSize().y / 2) * pixSize + getPositionWs().y, 0};
    }
    // 统一渲染时相机位置
    void renderFix() { positionForRender = getPositionWs(); }
    void drawSpr(GSprite *spr_, WindowBase &window_, const FVector3 &posWs_) {
        int winW = window_.getDefaultView().getSize().x;
        int winH = window_.getDefaultView().getSize().y;
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
class GMoveComponent : public GComponent {
    FVector3 moveVector = {0, 0, 0};
    FVector3 target = {0, 0, 0};

  public:
    float moveX = 0;
    float moveY = 0;
    float moveZ = 0;
    float speed = 0.03;
    FVector3 getMoveVector() { return moveVector; }
    void move(float deltaTime_) {
        moveVector = vectorNormalize({moveX, moveY, moveZ});
        owner->addPositionOffsetWs(moveVector * speed * deltaTime_);
        moveX = 0;
        moveY = 0;
        moveZ = 0;
    }
    bool isAutoMove = false;
    void autoMove(float deltaTime_) {
        const FVector3 &vec = target - owner->getPositionWs();
        float len = getVectorLen(vec);
        if (len < 2) {
            moveVector = {0, 0, 0};
            isAutoMove = false;
        } else {
            moveVector = vectorNormalize(vec);
        }
        owner->addPositionOffsetWs(moveVector * speed * deltaTime_);
    }
    void setTarget(const FVector3 &target_) {
        target = target_;
        isAutoMove = true;
    }
    void loop(WindowBase &window_, float deltaTime_) override {
        if (isAutoMove) {
            autoMove(deltaTime_);
        } else {
            move(deltaTime_);
        }
    }
};
class GAnimationSystemComponent : public GRenderObjComponent {};
class GPlayer : public GActor {
  public:
    GCameraComponent *cameraComp = nullptr;
    GStaticSpriteComponent *sprComp = nullptr;
    GMoveComponent *moveComp = nullptr;
    GController controller;
    GTexture tex;
    GPlayer();
};
class GAnimActor : public GActor {
  public:
    GSympleAnimationComponent *sprComp = nullptr;
    GAnimActor() {}
    void construct(GTexture &tex_, int begin_ = 0, int end_ = 0, int frameSpeed_ = 15) {
        sprComp = createComponent<GSympleAnimationComponent>();
        sprComp->setAnim(tex_, begin_, end_);
        sprComp->getAnimation().setFramePerS(frameSpeed_);
    }
};
class GStaticActor : public GActor {
  public:
    GTexture tex_;
    GStaticSpriteComponent *sprComp = nullptr;
    GStaticActor() {}
    void construct(GTexture &tex_, int id_) {
        sprComp = createComponent<GStaticSpriteComponent>();
        sprComp->getSprite().init(tex_);
        sprComp->getSprite().setId(id_);
    }
};
class GameMode : public GObject {
  public:
    GPlayer *player = nullptr;
};

class GWorld : public GObject {
  private:
    GameMode gm;
    struct ActorsType {};
    // std::vector<GActor *> allActorsActive;
    GridMap<GActor>gridMap;
    std::vector<GSprite *> allRenderObj;
    GCamera cameraDefault;
    GCameraComponent *cameraActive = nullptr;
    TimeManager timeManager;
    GController controllerDefault;
    GController *controllerActive = nullptr;
    float deltaTime = 0.1;
    sf::Clock clock;
    void bindDefaultCameraController() {
        cameraActive = (cameraDefault.cameraComp);
        controllerActive = &controllerDefault;
        controllerDefault.bind(GController::w, [&]() { cameraDefault.setPositionWs(cameraDefault.getPositionWs() + FVector3(0, -0.1, 0)); });
        controllerDefault.bind(GController::s, [&]() { cameraDefault.setPositionWs(cameraDefault.getPositionWs() + FVector3(0, 0.1, 0)); });
        controllerDefault.bind(GController::a, [&]() { cameraDefault.setPositionWs(cameraDefault.getPositionWs() + FVector3(-0.1, 0, 0)); });
        controllerDefault.bind(GController::d, [&]() { cameraDefault.setPositionWs(cameraDefault.getPositionWs() + FVector3(0.1, 0, 0)); });
    }

  public:
    template <class playerClass> GameMode& setGameMode() {
        //如果gm.player不为空，应先释放
        gm.player = createActor<playerClass>();
        cameraActive = (gm.player->cameraComp);
        controllerActive = &(gm.player->controller);
        return gm;
    }
    template <class T> T *createActor() {
        T *actor = new T();
        actor->actorBaseInit(this);
        // allActorsActive.push_back(actor);
        gridMap.addActor(actor);
        return actor;
    }
    std::vector<GSprite *> &getRenderObjComps() { return allRenderObj; }
    GCameraComponent *getCameraActive() { return cameraActive; }
    GController *getControllerActive() { return controllerActive; }

    GWorld() {
      bindDefaultCameraController();
      gridMap.init({-100,-100}, 50, 50, 10, 10);

        

        // test
    }
    void pollActorsActive(WindowBase &window_) {
        // std::sort(allActorsActive.begin(), allActorsActive.end(), [](GActor *a, GActor *b) { return a->getPositionWs().y < b->getPositionWs().y; });
        int centerId=gridMap.getPositionIndex(cameraActive->getPositionWs());
        gridMap.setActorsAlive(centerId);
        for (GActor *actor : gridMap.actorsAlive) {
            actor->loop(deltaTime, window_);
        }
        std::sort(allRenderObj.begin(), allRenderObj.end(), [](GSprite *a, GSprite *b) { return a->posWs.y < b->posWs.y; });
        for (GSprite *robj : allRenderObj) {
            cameraActive->drawSpr(robj, window_, robj->posWs);
        }
        allRenderObj.resize(0);
    }
    void loop(WindowBase &window_, EventBase &event_) {
        deltaTime = clock.restart().asSeconds();
        controllerActive->loop(window_, event_);

        //  计时器任务
        timeManager.loop();
        cameraActive->renderFix();
        // 渲染
        window_.clear(sf::Color::Black);
        // actor逻辑
        pollActorsActive(window_);
        // UI逻辑
        //
        // debug
        GDebug::debugDisplay(window_);
        window_.display();
    }
    ~GWorld() {
        
    }
};
class LevelManager : public GObject {};
class GGame : GObject {
  private:
    GWorld *curWorld = nullptr;

    sf::Event event;

  public:
    WindowBase window;
    template <class T>GWorld* createWorld() {
        delete curWorld;
        curWorld = new T;
        return curWorld;
    }

    static GGame *getGameIns() {
        static GGame game;
        return &game;
    }
    GGame() {
        window.create(sf::VideoMode(800, 600), "Game");
        window.setFramerateLimit(60);
        sf::Image icon;
        icon.loadFromFile("res/a.png");
        window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    }
    ~GGame() { delete curWorld; }
    void loop() {

        while (window.isOpen()) {

            curWorld->loop(window, event);
        }
    }
};

#endif // FRAMEWORK_H