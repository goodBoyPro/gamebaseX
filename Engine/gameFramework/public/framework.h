#ifndef FRAMEWORK_H
#define FRAMEWORK_H
#include "GComman.h"
#include "controllerX.h"
#include "gridWorld.h"
#include "heads.h"
#include "render/gsource.h"
#include "render/sprite.h"
#include <GDebug.h>
#include <fstream>
#include <nlohmann_json/json.hpp>
#include <timeManager.h>

class GObject {
public:
  bool isValid = true;
  virtual ~GObject() {};
};
///////////////////////////////////////////////////////////////////////////////////////////
class GComponent : public GObject {
protected:
  class GActor *owner = nullptr;

private:
public:
  GComponent() {}
  virtual void init(GActor *owner_) { owner = owner_; }

  virtual void loop(WindowBase &window_, float deltaTime_) {}
};
///////////////////////////////////////////////////////////////////////////////////////////
class GSceneComponent : public GComponent {
private:
  std::vector<GSceneComponent *> __childs;
  FVector3 positionRelative = {0, 0, 0};
  FVector3 sizeWs = {50, 50, 50};
  GSceneComponent *parentComp = nullptr;

public:
  void setPositionRelative(const FVector3 &posRelative_) {
    positionRelative = posRelative_;
  }
  void attachToComponent(GSceneComponent *parent_) { parentComp = parent_; }
  FVector3 &getPositionRelative() { return positionRelative; }
  FVector3 getPositionWs();
  void setPositionWs(const FVector3 &posWs_);
  FVector3 getSizeWs() { return sizeWs; }
  void setSizeWs(const FVector3 &sizeWs_) { sizeWs = sizeWs_; }
};
///////////////////////////////////////////////////////////////////////////////////////////
class GPrimitiveComponent : public GSceneComponent {
public:
  GSprite xSpr;
  GTexture xTex;
  GPrimitiveComponent() {
    xTex.init(1, 1, 0.5, 1, "system/texture/x.png");
    xSpr.init(xTex);
  }
  
};
///////////////////////////////////////////////////////////////////////////////////////////
class GRenderObjComponent : public GSceneComponent {
private:
  GSprite *sprite = nullptr;

public:
  void setRenderSpr(GSprite *spr_) { sprite = spr_; }
  GSprite *getRenderSpr() { return sprite; }
  void setSizeWin(float width_, float height_) {
    sprite->setSizeWin(width_, height_);
  }
  virtual void loop(WindowBase &window_, float deltaTime_) override;
};
///////////////////////////////////////////////////////////////////////////////////////////
class GStaticSpriteComponent : public GRenderObjComponent {
  GSprite spr;

public:
  GStaticSpriteComponent() { setRenderSpr(&spr); }
  void setTex(const GTexture &tex) { spr.init(tex); }
  GSprite &getSprite() { return spr; }
  GStaticSpriteComponent(GTexture &tex) { setTex(tex); }
};
///////////////////////////////////////////////////////////////////////////////////////////
class GSympleAnimationComponent : public GRenderObjComponent {
public:
  GAnimation anim;
  GSympleAnimationComponent() {
    setRenderSpr(&anim);
    anim.play();
  }
  GAnimation &getAnimation() { return anim; }
  void setAnim(GTexture &tex, int beg_, int end_) {
    anim.init(tex, beg_, end_);
  }
  void loop(WindowBase &window_, float deltaTime_) override {
    GRenderObjComponent::loop(window_, deltaTime_);
    anim.loop(deltaTime_);
  }
};
///////////////////////////////////////////////////////////////////////////////////////////
class GActor : public GObject {
private:
private:
  std::vector<GComponent *> __allComponents;
  GSceneComponent *rootComponent = nullptr;
  FVector3 positionWs = {0, 0, 0};

  class GWorld *worldPtr = nullptr;

public:
  int nodeId;
  virtual void beginPlay() {}
  virtual void tick() {}
  GActor() {};
  void actorBaseInit(GWorld *worldPtr_) { worldPtr = worldPtr_; }
  GWorld *getWorld() { return worldPtr; }
  virtual void loop(float deltatime_, WindowBase &window_);
  const FVector3 &getPositionWs() const { return positionWs; }
  void setPositionWs(const FVector3 &posWs_);
  void addPositionOffsetWs(const FVector3 &offset) {
    setPositionWs(positionWs + offset);
  }
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
  void destroyActor() { isValid = false; };
  ~GActor() {
    for (GComponent *comp : __allComponents) {
      delete comp;
    }
  };

public:
  static void loopAllActorsActive(std::vector<GActor *> allActorsActive_,
                                  float deltatime_,
                                  class GCameraComponent *camera_,
                                  WindowBase &window_) {
    for (GActor *actor : allActorsActive_) {
      actor->loop(deltatime_, window_);
    }
  }
};
///////////////////////////////////////////////////////////////////////////////////////////
class GCameraComponent : public GSceneComponent {
private:
  float pixSize = 0.01;
  FVector3 positionForRender;
  FVector3 wsToWin(const FVector3 &PositionInWS, float winW_, float win_H) {
    return {((PositionInWS.x - getPositionWs().x) / pixSize + winW_ / 2.f),
            ((PositionInWS.y - getPositionWs().y) / pixSize + win_H / 2.f -
             (PositionInWS.z / pixSize)),
            0};
  }
  void renderFix() { positionForRender = getPositionWs(); }
  void drawSpr(GRenderObjComponent *spr_, WindowBase &window_);
  std::vector<sf::Vertex> points;
  WindowBase*window=nullptr;

public:
  void setWindow(WindowBase*window_){window=window_;}
  WindowBase* getWindow(){return window;}
  void setPixSize(float pSize_) { pixSize = pSize_; }
  float getPixSize() { return pixSize; }
  FVector3 winToWs(const IVector2 &posWin_, WindowBase &window_) {
    return {(posWin_.x - window_.getDefaultView().getSize().x / 2) * pixSize +
                getPositionWs().x,
            (posWin_.y - window_.getDefaultView().getSize().y / 2) * pixSize +
                getPositionWs().y,
            0};
  }
  FVector3 getMousePositionWs(WindowBase &window_) {
    const IVector2 &posWin = sf::Mouse::getPosition(window_);
    IVector2 posfix = {(int)(posWin.x * window_.getDefaultView().getSize().x /
                             window_.getSize().x),
                       (int)(posWin.y * window_.getDefaultView().getSize().y /
                             window_.getSize().y)};

    return winToWs(posfix, window_);
  };

  void drawLineWs(const std::vector<FVector3> &pointsVector_,
                  WindowBase &window_, ColorBase color = ColorBase::Red) {
    points.resize(0);
    int n = 0;
    int winW = window_.getDefaultView().getSize().x;
    int winH = window_.getDefaultView().getSize().y;
    for (const FVector3 &pos : pointsVector_) {
      const FVector3 &poswin = wsToWin(pos, winW, winH);
      points.emplace_back(sf::Vertex({poswin.x, poswin.y}, color));
      if (n)
        points.emplace_back(sf::Vertex({poswin.x, poswin.y}, color));
      n++;
    }

    window_.draw(points.data(), points.size(), sf::Lines);
  }
  // 统一渲染时相机位置

  void drawAllRenDerObj(std::vector<GRenderObjComponent *> rObjs,
                        WindowBase &window_);
};
///////////////////////////////////////////////////////////////////////////////////////////
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
  float speed = 1;
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
    if (len < 0.01) {
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
///////////////////////////////////////////////////////////////////////////////////////////
class GAnimationSystemComponent : public GRenderObjComponent {};
///////////////////////////////////////////////////////////////////////////////////////////
class GPlayer : public GActor {
public:
  GCameraComponent *cameraComp = nullptr;
  GStaticSpriteComponent *sprComp = nullptr;
  GMoveComponent *moveComp = nullptr;
  GController controller;

  GPlayer();
  void beginPlay() override;
};
class GAnimActor : public GActor {
public:
  GSympleAnimationComponent *sprComp = nullptr;
  GAnimActor() {}
  void construct(GTexture &tex_, int begin_ = 0, int end_ = 0,
                 int frameSpeed_ = 15) {
    sprComp = createComponent<GSympleAnimationComponent>();
    sprComp->setAnim(tex_, begin_, end_);
    sprComp->getAnimation().setFramePerS(frameSpeed_);
  }
};
///////////////////////////////////////////////////////////////////////////////////////////
class GStaticActor : public GActor {
public:
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
///////////////////////////////////////////////////////////////////////////////////////////
class GWorld : public GObject {
private:
  GSource *source = nullptr;
  GameMode gm;

  struct ActorsType {};
  // std::vector<GActor *> allActorsActive;
  GridMap<GActor> gridMap;
  friend void GActor::setPositionWs(const FVector3 &posWs_);
  std::vector<GRenderObjComponent *> allRenderObj;
  GCamera cameraDefault;
  GCameraComponent *cameraActive = nullptr;
  TimeManager timeManager;
  GController controllerDefault;
  GController *controllerActive = nullptr;
  float deltaTime = 0.1;
  sf::Clock clock;
  void bindDefaultCameraController();

public:
  GSource *getSource() { return source; };
  template <class playerClass> GameMode &setGameMode() {
    // 如果gm.player不为空，应先释放
    gm.player = createActor<playerClass>();
    cameraActive = (gm.player->cameraComp);
    controllerActive = &(gm.player->controller);
    return gm;
  }
  void setCameraActive(GCameraComponent *camera_);
  template <class T> T *createActor(const FVector3 &position = {0, 0, 0}) {
    T *actor = new T();
    // 特别注意，init是在构造函数之后执行的，子类要慎重写构造函数，推荐beginPlay()
    actor->actorBaseInit(this);
    int nodeId = gridMap.addActor(actor);
    actor->nodeId = nodeId;
    actor->setPositionWs(position);
    actor->beginPlay();
    return actor;
  }
  std::vector<GRenderObjComponent *> &getRenderObjComps() {
    return allRenderObj;
  }
  GCameraComponent *getCameraActive() { return cameraActive; }
  GController *getControllerActive() { return controllerActive; }
  TimeManager &getTimeManager() { return timeManager; }
  void loadBaseActors(const std::string &jsonPath_);
  void showGridMap(WindowBase &window_);
  GWorld() {
    source = new GSource();
    bindDefaultCameraController();
    gridMap.init({-100, -100}, 50, 50, 10, 10);

    // test
  }
  virtual void tick();
  virtual void beginPlay() {}
  void pollActorsActive(WindowBase &window_);
  void loop(WindowBase &window_, EventBase &event_);
  ~GWorld() { delete source; }
};
class LevelManager : public GObject {};
class GGame : GObject {
private:
  GWorld *curWorld = nullptr;

  sf::Event event;

public:
  WindowBase window;
  template <class T> GWorld *createWorld() {
    delete curWorld;
    curWorld = new T;
    curWorld->beginPlay();
    return curWorld;
  }

  static GGame *getGameIns() {
    static GGame game;
    return &game;
  }
  GGame() {
    window.create(sf::VideoMode(1600, 900), "Game");
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