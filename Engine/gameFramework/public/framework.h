#ifndef FRAMEWORK_H
#define FRAMEWORK_H
#include "GComman.h"
#include "controllerX.h"
#include "gameConfig.h"
#include "gridWorld.h"
#include "render/gameShader.h"
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
  } autoRegister##className;                                                   \
// REGISTER_CLASS(className)
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
  }                                                                            \
                                                                               \
private:                                                                       \
// REGISTER_BODY(className)

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
  static GObject *constructObject(const std::string &className_);
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
  virtual void construct(GActor *owner_) { owner = owner_; }

  virtual void loop(GameWindow &window_, float deltaTime_) {}
  virtual void tick() {}
  virtual void beginPlay() {}
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
  void setPositionRelative(const FVector3 &posRelative_);
  void attachToComponent(GSceneComponent *parent_);
  FVector3 &getPositionRelative();
  FVector3 getPositionWs();
  void setPositionWs(const FVector3 &posWs_);
  FVector3 getSizeWs();
  void setSizeWs(const FVector3 &sizeWs_);
};
///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GPrimitiveComponent)
class GPrimitiveComponent : public GSceneComponent {
  REGISTER_BODY(GPrimitiveComponent)
public:
  GPrimitiveComponent() {}

  virtual void loop(GameWindow &window_, float deltaTime_) override {}
};

///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GRenderObjComponent)
class GRenderObjComponent : public GSceneComponent {
  REGISTER_BODY(GRenderObjComponent)
private:
  GSprite *sprite = nullptr;

public:
  void setRenderSpr(GSprite *spr_);
  GSprite *getRenderSpr();
  void setSizeWin(float width_, float height_);
  virtual void loop(GameWindow &window_, float deltaTime_) override;
};
///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GStaticSpriteComponent)
class GStaticSpriteComponent : public GRenderObjComponent {

  GSprite spr;
  REGISTER_BODY(GStaticSpriteComponent)
public:
  GStaticSpriteComponent();
  void setTex(GTexture &tex);
  GSprite &getSprite();
  GStaticSpriteComponent(GTexture &tex);
};
///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GSympleAnimationComponent)
class GSympleAnimationComponent : public GRenderObjComponent {
  REGISTER_BODY(GSympleAnimationComponent)
public:
  GAnimation anim;
  GSympleAnimationComponent();
  GAnimation &getAnimation();
  void setAnim(GTexture &tex, int beg_, int end_);
  void loop(GameWindow &window_, float deltaTime_) override;
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
#ifdef EDITOR
  bool isEditorObj = false;
#endif
  int nodeId = -1;
  virtual void beginPlay() {}
  virtual void tick() {}
  GActor();

  void actorBaseInit(GWorld *worldPtr_);
  GWorld *getWorld();
  virtual void loop(float deltatime_, GameWindow &window_);
  const FVector3 &getPositionWs() const;
  void setPositionWs(const FVector3 &posWs_);
  void addPositionOffsetWs(const FVector3 &offset);
  template <class T> T *createComponent() {
    GComponent *comp = new T();
    comp->construct(this);
    comp->beginPlay();
    __allComponents.push_back(comp);
    return (T *)comp;
  }
  void setRootComponent(GSceneComponent *comp_);
  bool isActive = false;
  void setActive();
  void disableActive();
  void destroyActor();
  ~GActor();

public:
  static void loopAllActorsActive(std::vector<GActor *> allActorsActive_,
                                  float deltatime_, class GCameraObj *camera_,
                                  GameWindow &window_);
};
///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GCameraObj)
class GCameraObj : public GObject {
  REGISTER_BODY(GCameraObj)
private:
  FVector3 positionWs = {0, 0, 0};
  float pixSize = 0.01;
  FVector3 positionForRender;

  void renderFix();
  void drawSpr(GRenderObjComponent *spr_, GameWindow &window_);
  std::vector<sf::Vertex> points;
  GameWindow *window = nullptr;

public:
  GSceneComponent *parentComp = nullptr;
  FVector3 getPositionWs();
  void setPositionWs(const FVector3 &posWs_);
  // 用户最好不要调用，请使用GameWindow中提供的方法
  IVector2 wsToWin(const FVector3 &PositionInWS, float winW_, float win_H);
  void setWindow(GameWindow *window_);
  GameWindow *getWindow();
  void setPixSize(float pSize_);
  float getPixSize();
  // 用户最好不要调用，请使用GameWindow中提供的方法
  FVector3 winToWs(const FVector2 &posWin_, GameWindow &window_);
  // 用户最好不要调用，请使用GameWindow中提供的方法
  FVector3 getMousePositionWs(GameWindow &window_);
  void drawLineWin(const std::vector<FVector2> &points_, GameWindow &window_,
                   ColorBase color = ColorBase::Red);
  void drawLineWs(const std::vector<FVector3> &pointsVector_,
                  GameWindow &window_, ColorBase color = ColorBase::Red);
  // 统一渲染时相机位置

  void drawAllRenDerObj(std::vector<GRenderObjComponent *> rObjs,
                        GameWindow &window_);
};
///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GCameraComponent)
class GCameraComponent : public GSceneComponent {
  REGISTER_BODY(GCameraComponent)
public:
  GCameraObj camera;
  GCameraComponent();
};
///////////////////////////////////////////////////////////////////////////////////////////
class GMoveComponent : public GComponent {
  FVector3 moveVector = {0, 0, 0};
  FVector3 target = {0, 0, 0};

public:
  float moveX = 0;
  float moveY = 0;
  float moveZ = 0;
  float speed = 1;
  FVector3 getMoveVector();
  void move(float deltaTime_);
  bool isAutoMove = false;
  void autoMove(float deltaTime_);
  void setTarget(const FVector3 &target_) {
    target = target_;
    isAutoMove = true;
  }
  void loop(GameWindow &window_, float deltaTime_) override;
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
///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GAnimActor)
class GAnimActor : public GActor {
  REGISTER_BODY(GAnimActor)
public:
  GSympleAnimationComponent *sprComp = nullptr;
  GAnimActor() {}
  void construct(GTexture &tex_, int begin_ = 0, int end_ = 0,
                 int frameSpeed_ = 15);
};
///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GStaticActor)
class GStaticActor : public GActor {
  REGISTER_BODY(GStaticActor)
public:
public:
  class StaticActorInfo *infoPtr = nullptr;
  GStaticSpriteComponent *sprComp = nullptr;
  GStaticActor() {}
  void construct(GTexture &tex_, int id_);
};
//////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GameMode)
class GameMode : public GObject {
  REGISTER_BODY(GameMode)
public:
  GPlayer *player = nullptr;
  class GGame *gameIns = nullptr;
};
///////////////////////////////////////////////////////////////////////////////////////////
class GLandScape : public GSprite {
  float beginX;
  float beginY;
  float widhtTotal;
  float heightTotal;
  GMaterial landMaterial;

public:
  GSprite spr;
  GMaterial &getMaterial();
  void init(float beginX_, float beginY_, float widthTotal_, float heightTotal_,
            const std::string &matPath_);

  void draw(GameWindow &window_);
};
////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(GWorld)
class GWorld : public GObject {
  REGISTER_BODY(GWorld)
private:
  void asyncLoad(std::string jsonPath_);
  std::atomic<bool> isDataLoadComplete = false;
  struct ActorsType {};

  GridMap<GActor> gridMap;
  friend void GActor::setPositionWs(const FVector3 &posWs_);
  std::vector<GRenderObjComponent *> allRenderObj;
  GCameraObj cameraDefault;
  TimeManager timeManager;
  GController controllerDefault;
  GController *controllerActive = nullptr;
  float deltaTime = 0.1;
  sf::Clock clock;
  void bindDefaultCameraController();
  struct ActorContext {
    GWorld *______worldParamForCreate;
  };
  inline thread_local static ActorContext actorContext;

public:
  bool isLoadComplete() { return isDataLoadComplete; }
  GCameraObj &getCameraDefault() { return cameraDefault; }
  GController *getControllerDefault() { return &controllerDefault; }

  void useDefaultControllerAndCamera();
  void setControllerActive(GController *controller_);

  friend class GActor;
  friend class GGame;
  GridMap<GActor> &getGridMap();
  void setActorContext();

  GameMode gm;
  GLandScape landScape;

  // 该函数要访问gameIns，但是gameIns不是在构造函数设置的，如果在构造函数使用此函数，会出错
  GameMode &setGameMode(const std::string &playerClass_);
  void setCameraActive(GCameraObj *camera_);
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
                                 const FVector3 &position = {0, 0, 0});
  std::vector<GRenderObjComponent *> &getRenderObjComps();
  GCameraObj *getCameraActive();
  GController *getControllerActive();
  TimeManager &getTimeManager();
  void loadBaseActors(const std::string &jsonPath_);
  void showGridMap(GameWindow &window_);
  GWorld() {

    // test
  }
  void Construct() {}
  virtual void tick();
  virtual void beginPlay() {}
  void pollActorsActive(GameWindow &window_);
  virtual void loop(GameWindow &window_, EventBase &event_);
  virtual void render(GameWindow &window_, EventBase &event_);
  ~GWorld() {}
  GStaticActor *createStaticActor(const std::string &name_,
                                  const FVector3 &pos_ = {0, 0, 0},
                                  const FVector3 &sizeWs_ = {1, 1, 1});
};
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
class PageGameWaitSourceLoad : public GWorld {
public:
  PageGameWaitSourceLoad() { getGridMap().init(1, 1, 1000, 1000); }
  std::function<void()> doSomethingBoforeToWorld = []() {};
  void loop(GameWindow &window_, EventBase &event_) override;
};
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
REGISTER_CLASS(LevelManager)
class LevelManager : public GObject {
  REGISTER_BODY(LevelManager)
};
REGISTER_CLASS(GGame)
class GGame : public GObject {
  REGISTER_BODY(GGame)
public:
  GWorld *curWorld = nullptr;
  GWorld *worldLoading = nullptr;
  PageGameWaitSourceLoad waitPage;
  EventBase event;

public:
  void setCameraActive(GCameraObj *camera_);
  GameWindow window;
  template <class T> GWorld *loadWorld(const std::string &jsonPath_ = "") {
    delete worldLoading;
    worldLoading = nullptr;
    curWorld = &waitPage;

    worldLoading = new T;

    worldLoading->gm.gameIns = this;
    worldLoading->bindDefaultCameraController();

    worldLoading->loadBaseActors(jsonPath_);
    worldLoading->Construct();
    worldLoading->beginPlay();
    return worldLoading;
  }
  template <class T>
  GWorld *createWorld(int rows, int colomns, float width, int height,
                      const std::string matJson_ = "") {
    curWorld = &waitPage;
    delete worldLoading;
    worldLoading = new T;
    worldLoading->gm.gameIns = this;
    worldLoading->bindDefaultCameraController();
    worldLoading->getGridMap().init(rows, colomns, width, height);

    worldLoading->bindDefaultCameraController();
    const std::vector<float> mapbs =
        worldLoading->getGridMap().getMapBeginPosAndTotalSize();
    if (matJson_ != "") {
      worldLoading->landScape.init(mapbs[0], mapbs[1], mapbs[2], mapbs[3],
                                   matJson_);
    }
    curWorld = worldLoading;
    return worldLoading;
  }

  GGame();
  ~GGame();
  virtual void loop();
};

inline GameMode &GWorld::setGameMode(const std::string &playerClass_) {

  gm.player = (GPlayer *)createActorByClassName(playerClass_);
  if (gm.gameIns) {
    gm.gameIns->window.setCameraActive(&(gm.player->cameraComp->camera));
  } else {
    printf("WARNING:setGameMode()cannot be used in construct\n");
  }

  controllerActive = &(gm.player->controller);

  return gm;
}
#endif // FRAMEWORK_Hvoid WorldEditorWindow::setWin1(){};void
       // WorldEditorWindow::setMenus(){};