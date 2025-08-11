#ifndef FRAMEWORK_H
#define FRAMEWORK_H
#include "GComman.h"
#include "controllerX.h"
#include "gridWorld.h"
#include "render/gsource.h"
#include "render/sprite.h"
#include <GDebug.h>
#include <gameLog.h>
#include <nlohmann_json/json.hpp>
#include <timeManager.h>

// 反射
#define REGISTER_CLASS(className)                                              \
  class className;                                                             \
  inline struct AutoRegister##className {                                      \
    AutoRegister##className() { GClass::regClass<className>(#className); }     \
  } autoRegister##className;

class GClass {
public:
  static std::unordered_map<std::string, GClass> &getClassRegInfo() {
    static std::unordered_map<std::string, GClass> inst;
    return inst;
  }

  friend class GObject;
  std::function<class GObject *()> constructCbk;
  std::string className;
  template <class T> static void regClass(const std::string &className_) {
    if (getClassRegInfo().find(className_) != getClassRegInfo().end()) {
      throw std::overflow_error(className_ + ":class redefined\n");
    }
    GAMELOG(std::string("register class:").append(className_));
    getClassRegInfo()[className_].className = className_;
    getClassRegInfo()[className_].constructCbk = []() -> GObject * {
      return new T; // 释放流程待添加
    };
  }
};
#define REGISTER_BODY(className)                                               \
public:                                                                        \
  virtual GClass &getGClass() override {                                       \
    return GClass::getClassRegInfo()[#className];                              \
  }

///////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GObject)
class GObject {
  // 反射主体
public:
  virtual GClass &getGClass() { return GClass::getClassRegInfo()["GObject"]; }

private:
public:
  bool isValid = true;
  virtual ~GObject() {};
  static GObject *constructObject(const std::string &className_) {
    if (GClass::getClassRegInfo().find(className_) ==
        GClass::getClassRegInfo().end()) {
      throw std::overflow_error(
          className_ + ":constructObject use unRegistered className\n");
    }
    return GClass::getClassRegInfo()[className_].constructCbk();
  }
};
///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GComponent)
class GComponent : public GObject {
  REGISTER_BODY(GComponent)
protected:
  class GActor *owner = nullptr;

private:
public:
  GComponent() {}
  virtual void init(GActor *owner_) { owner = owner_; }

  virtual void loop(GameWindow &window_, float deltaTime_) {}
};
///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GSceneComponent)
class GSceneComponent : public GComponent {
  REGISTER_BODY(GSceneComponent)
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
REGISTER_CLASS(GPrimitiveComponent)
class GPrimitiveComponent : public GSceneComponent {
  REGISTER_BODY(GPrimitiveComponent)
public:
  inline static GSprite xSpr;
  inline static GTexture xTex;
  inline static GSprite ySpr;
  inline static GTexture yTex;
  inline static GSprite zSpr;
  inline static GTexture zTex;
  inline static GSprite cSpr;
  inline static GTexture cTex;
  GPrimitiveComponent() { static bool temp = createAxis(); }
  bool createAxis() {
    xTex.init(1, 1, 0.5, 1, "system/texture/x.png");
    xSpr.init(xTex);
    yTex.init(1, 1, 0.5, 1, "system/texture/y.png");
    ySpr.init(yTex);
    zTex.init(1, 1, 0.5, 1, "system/texture/z.png");
    zSpr.init(zTex);
    cTex.init(1, 1, 0.5, 1, "system/texture/c.png");
    cSpr.init(cTex);
    xSpr.setSizeWin(10, 100);
    xSpr.setCenter(0.5, 1.3);
    xSpr.setRotation(90);
    ySpr.setSizeWin(10, 100);
    ySpr.setCenter(0.5, 1.3);
    zSpr.setSizeWin(10, 100);
    zSpr.setCenter(0.5, 1.4246);
    zSpr.setRotation(45);
    cSpr.setSizeWin(30, 30);
    cSpr.setCenter(0.1, 0.9);

    return true;
  }
  virtual void loop(GameWindow &window_, float deltaTime_) override {
    draw(window_);
  }
  void draw(GameWindow &window_);
};

///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GRenderObjComponent)
class GRenderObjComponent : public GSceneComponent {
  REGISTER_BODY(GRenderObjComponent)
private:
  GSprite *sprite = nullptr;

public:
  void setRenderSpr(GSprite *spr_) { sprite = spr_; }
  GSprite *getRenderSpr() { return sprite; }
  void setSizeWin(float width_, float height_) {
    sprite->setSizeWin(width_, height_);
  }
  virtual void loop(GameWindow &window_, float deltaTime_) override;
};
///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GStaticSpriteComponent)
class GStaticSpriteComponent : public GRenderObjComponent {

  GSprite spr;
  REGISTER_BODY(GStaticSpriteComponent)
public:
  GStaticSpriteComponent() { setRenderSpr(&spr); }
  void setTex(const GTexture &tex) { spr.init(tex); }
  GSprite &getSprite() { return spr; }
  GStaticSpriteComponent(GTexture &tex) { setTex(tex); }
};
///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GSympleAnimationComponent)
class GSympleAnimationComponent : public GRenderObjComponent {
  REGISTER_BODY(GSympleAnimationComponent)
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
  void loop(GameWindow &window_, float deltaTime_) override {
    GRenderObjComponent::loop(window_, deltaTime_);
    anim.loop(deltaTime_);
  }
};
///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GActor)
class GActor : public GObject {
  REGISTER_BODY(GActor)
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
  GActor();
  void actorBaseInit(GWorld *worldPtr_) { worldPtr = worldPtr_; }
  GWorld *getWorld() { return worldPtr; }
  virtual void loop(float deltatime_, GameWindow &window_);
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
                                  GameWindow &window_) {
    for (GActor *actor : allActorsActive_) {
      actor->loop(deltatime_, window_);
    }
  }
};
///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GCameraComponent)
class GCameraComponent : public GSceneComponent {
  REGISTER_BODY(GCameraComponent)
private:
  float pixSize = 0.01;
  FVector3 positionForRender;

  void renderFix() { positionForRender = getPositionWs(); }
  void drawSpr(GRenderObjComponent *spr_, GameWindow &window_);
  std::vector<sf::Vertex> points;
  GameWindow *window = nullptr;

public:
  FVector3 wsToWin(const FVector3 &PositionInWS, float winW_, float win_H) {
    return {((PositionInWS.x - getPositionWs().x) / pixSize + winW_ / 2.f),
            ((PositionInWS.y - getPositionWs().y) / pixSize + win_H / 2.f -
             (PositionInWS.z / pixSize)),
            0};
  }
  void setWindow(GameWindow *window_) { window = window_; }
  GameWindow *getWindow() { return window; }
  void setPixSize(float pSize_) { pixSize = pSize_; }
  float getPixSize() { return pixSize; }
  FVector3 winToWs(const IVector2 &posWin_, GameWindow &window_) {
    return {(posWin_.x - window_.getDefaultView().getSize().x / 2) * pixSize +
                getPositionWs().x,
            (posWin_.y - window_.getDefaultView().getSize().y / 2) * pixSize +
                getPositionWs().y,
            0};
  }
  FVector3 getMousePositionWs(GameWindow &window_) {
    const IVector2 &posWin = sf::Mouse::getPosition(window_);
    IVector2 posfix = {(int)(posWin.x * window_.getDefaultView().getSize().x /
                             window_.getSize().x),
                       (int)(posWin.y * window_.getDefaultView().getSize().y /
                             window_.getSize().y)};

    return winToWs(posfix, window_);
  };

  void drawLineWs(const std::vector<FVector3> &pointsVector_,
                  GameWindow &window_, ColorBase color = ColorBase::Red) {
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
                        GameWindow &window_);
};
///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GCamera)
class GCamera : public GActor {
  REGISTER_BODY(GCamera)
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
  void loop(GameWindow &window_, float deltaTime_) override {
    if (isAutoMove) {
      autoMove(deltaTime_);
    } else {
      move(deltaTime_);
    }
  }
};
///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GAnimationSystemComponent)
class GAnimationSystemComponent : public GRenderObjComponent {
  REGISTER_BODY(GAnimationSystemComponent)
};
///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GPlayer)
class GPlayer : public GActor {
  REGISTER_BODY(GPlayer)
public:
  GCameraComponent *cameraComp = nullptr;
  GStaticSpriteComponent *sprComp = nullptr;
  GMoveComponent *moveComp = nullptr;
  GController controller;

  GPlayer();
  void beginPlay() override;
};
REGISTER_CLASS(GAnimActor)
class GAnimActor : public GActor {
  REGISTER_BODY(GAnimActor)
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
REGISTER_CLASS(GStaticActor)
class GStaticActor : public GActor {
  REGISTER_BODY(GStaticActor)
public:
  GStaticSpriteComponent *sprComp = nullptr;
  GStaticActor() {}
  void construct(GTexture &tex_, int id_) {
    sprComp = createComponent<GStaticSpriteComponent>();
    sprComp->getSprite().init(tex_);
    sprComp->getSprite().setId(id_);
  }
};
REGISTER_CLASS(GameMode)
class GameMode : public GObject {
  REGISTER_BODY(GameMode)
public:
  GPlayer *player = nullptr;
  class GGame *gameIns = nullptr;
};
///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GWorld)
class GWorld : public GObject {
  REGISTER_BODY(GWorld)
private:
  GSource *source = nullptr;

  struct ActorsType {};
  // std::vector<GActor *> allActorsActive;
  GridMap<GActor> gridMap;
  friend void GActor::setPositionWs(const FVector3 &posWs_);
  std::vector<GRenderObjComponent *> allRenderObj;
  GCamera *cameraDefaultPtr;
  // GCameraComponent *cameraActive = nullptr;
  TimeManager timeManager;
  GController controllerDefault;
  GController *controllerActive = nullptr;
  float deltaTime = 0.1;
  sf::Clock clock;
  void bindDefaultCameraController();
  struct ActorContext {
    GWorld *______worldParamForCreate;
  };
  inline static ActorContext actorContext;

public:
  friend class GActor;
  void setActorContext() { actorContext.______worldParamForCreate = this; }

  GameMode gm;
  GSource *getSource() { return source; };
  // 该函数要访问gameIns，但是gameIns不是在构造函数设置的，如果在构造函数使用此函数，会出错
  template <class playerClass> GameMode &setGameMode();
  void setCameraActive(GCameraComponent *camera_);
  template <class T> T *createActor(const FVector3 &position = {0, 0, 0}) {
    setActorContext();
    T *actor = new T();
    // 1.先设置默认nodeId 2.设置新位置 3. 更新位置（此处是有意设计，不要修改）
    actor->nodeId = gridMap.addActor(actor);
    actor->setPositionWs(position);
    actor->beginPlay();
    return actor;
  }
  GActor *createActorByClassName(const std::string &className_,
                                 const FVector3 &position = {0, 0, 0}) {
    setActorContext();
    GActor *rtn = (GActor *)GObject::constructObject(className_);
    rtn->nodeId = gridMap.addActor(rtn);
    rtn->setPositionWs(position);
    rtn->beginPlay();
    return rtn;
  }
  std::vector<GRenderObjComponent *> &getRenderObjComps() {
    return allRenderObj;
  }
  GCameraComponent *getCameraActive();
  GController *getControllerActive() { return controllerActive; }
  TimeManager &getTimeManager() { return timeManager; }
  void loadBaseActors(const std::string &jsonPath_);
  void showGridMap(GameWindow &window_);
  GWorld() {

    source = new GSource();
    gridMap.init({-100, -100}, 50, 50, 10, 10);
    // test
  }
  void Construct() { bindDefaultCameraController(); }
  virtual void tick();
  virtual void beginPlay() {
    setGameMode<GPlayer>().player->moveComp->speed = 1;
  }
  void pollActorsActive(GameWindow &window_);
  void loop(GameWindow &window_, EventBase &event_);
  ~GWorld() { delete source; }
};
REGISTER_CLASS(LevelManager)
class LevelManager : public GObject {
  REGISTER_BODY(LevelManager)
};
REGISTER_CLASS(GGame)
class GGame : public GObject {
  REGISTER_BODY(GGame)
private:
  GWorld *curWorld = nullptr;

  sf::Event event;

public:
  GameWindow window;
  template <class T> GWorld *createWorld() {
    delete curWorld;
    curWorld = new T;
    curWorld->gm.gameIns = this;
    curWorld->Construct();
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

template <class playerClass> inline GameMode &GWorld::setGameMode() {
  // 如果gm.player不为空，应先释放
  delete gm.player;
  gm.player = createActor<playerClass>();
  if (gm.gameIns) {
    gm.gameIns->window.setCameraActive(gm.player->cameraComp);
  } else {
    printf("WARNING:setGameMode()cannot be used in construct\n");
  }

  controllerActive = &(gm.player->controller);

  return gm;
}
#endif // FRAMEWORK_H