
#ifndef WORLDEDITOR_H
#define WORLDEDITOR_H

#include "framework.h"
#include "gui/imguiDx/guiDx.h"
#include "render/sprite.h"

class MovableObj {
public:
  GSprite spr;
  FVector2 posPressedRelative = {0, 0};
  MovableObj() {}
  void init(GTexture &tex) { spr.init(tex); }
  void draw(GameWindow &window_, FVector2 &posWIn) {
    spr.setPositionWin(posWIn.x, posWIn.y);
    spr.drawWin(window_);
  }
  bool isContainMouse(GameWindow &window_) {
    const IVector2 &pos = (IVector2)window_.getMousePositionWin();
    return spr.isContainPos(pos.x, pos.y);
  }
  std::function<void(GameWindow &window_)> cbk;
};
class MovableAxis {
public:
  bool selected = false;
  GTexture tex[4];
  MovableObj *activeMO = nullptr;
  MovableObj axisX;
  MovableObj axisY;
  MovableObj axisZ;
  MovableObj center;
  FVector2 posInWin = {300, 300};
  float zPosWin = 0;
  float zPosLastTime = -1;
  float deltaMouseMoveZ = 0;
  void setZPosOffset(float offset_) { zPosWin += offset_; }
  FVector2 posInWinLast = {300, 300};
  FVector3 deltaMove;
  FVector3 &getDeltaPosWin() { return deltaMove; }
  MovableAxis() {

    tex[0].init(1, 1, 0.5, 1.3, "system/texture/x.png");
    tex[1].init(1, 1, 0.5, 1, "system/texture/y.png");
    tex[2].init(1, 1, 0.5, 1.3, "system/texture/z.png");
    tex[3].init(1, 1, 0.1, 0.9, "system/texture/c.png");
    axisX.init(tex[0]);
    axisX.spr.setSizeWin(10, 100);
    axisX.spr.setRotation(90);
    axisY.init(tex[1]);
    axisY.spr.setSizeWin(10, 50);
    axisY.spr.setRotation(180);
    axisZ.init(tex[2]);
    axisZ.spr.setSizeWin(10, 100);
    // axisZ.spr.setRotation(180);
    center.init(tex[3]);
    center.spr.setSizeWin(30, 30);
    axisX.cbk = [this](GameWindow &window_) {
      posInWin.x = window_.getMousePositionWin().x - axisX.posPressedRelative.x;
    };
    axisY.cbk = [this](GameWindow &window_) {
      posInWin.y = window_.getMousePositionWin().y - axisY.posPressedRelative.y;
    };
    axisZ.cbk = [this](GameWindow &window_) {
      float currentMousePos = window_.getMousePositionWin().y;
      if (zPosLastTime == -1) {
        zPosLastTime = currentMousePos;
        return;
      }
      deltaMouseMoveZ = currentMousePos - zPosLastTime;
      setZPosOffset(deltaMouseMoveZ);
      zPosLastTime = currentMousePos;
    };
    center.cbk = [this](GameWindow &window_) {
      posInWin =
          (FVector2)window_.getMousePositionWin() - center.posPressedRelative;
    };
  }
  void loop(GameWindow &window_, EventBase &event_) {
    PRINTDEBUG(L"zpos:%f", zPosWin);

    axisX.draw(window_, posInWin);
    axisY.draw(window_, posInWin);
    axisZ.draw(window_, posInWin);
    center.draw(window_, posInWin);
  }
  void listenEvent(GameWindow &window_, EventBase &event_) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      const FVector2 &posRelative =
          (FVector2)window_.getMousePositionWin() - posInWin;
      if (activeMO == nullptr) {
        if (axisX.isContainMouse(window_)) {
          activeMO = &axisX;
          axisX.posPressedRelative = posRelative;
        } else if (axisY.isContainMouse(window_)) {
          activeMO = &axisY;
          axisY.posPressedRelative = posRelative;
        } else if (axisZ.isContainMouse(window_)) {
          activeMO = &axisZ;
        } else if (center.isContainMouse(window_)) {
          activeMO = &center;
          center.posPressedRelative = posRelative;
        } else {
          selected = false;
        }
      }
    } else {
      activeMO = nullptr;
      zPosLastTime = -1;
      deltaMouseMoveZ = 0;
    }

    if (activeMO) {
      posInWinLast = posInWin;
      activeMO->cbk(window_);
      const FVector2 &xyPos = posInWin - posInWinLast;
      deltaMove = {xyPos.x, xyPos.y, -deltaMouseMoveZ};
    }
  }
};

///////////////test
REGISTER_CLASS(MyActor)
class MyActor : public GStaticActor {
  REGISTER_BODY(MyActor)
public:
  FVector3 direction;
  float speed = 0.8;
  int timeHandle;
  void tick() override { setPositionWs(getPositionWs() + direction * speed); }
  void beginPlay() override {
    timeHandle = getWorld()->getTimeManager().addTask(5000, 1, [this]() {
      getWorld()->getTimeManager().cancelTask(timeHandle);
      destroyActor();
    });
  }
};
///////////////
class RectForEditor {
public:
  sf::ConvexShape shape;
  sf::Color color = {255, 255, 0, 255};
  sf::Color fillColor = {0, 0, 0, 0};
  IVector2 posInWin = {100, 100};
  FVector2 size = {100, 100};
  float lineThickness = 3;
  bool containPos(const IVector2 &posWIn_) {
    return shape.getGlobalBounds().contains(posWIn_.x, posWIn_.y);
  }
  RectForEditor() {
    shape.setPointCount(4);
    shape.setPoint(0, {0, 0});
    shape.setPoint(1, {size.x, 0});
    shape.setPoint(2, {size.x, size.y});
    shape.setPoint(3, {0, size.y});
    shape.setPosition(posInWin.x, posInWin.y);
    shape.setFillColor(fillColor);
    shape.setOutlineColor(color);
    shape.setOutlineThickness(lineThickness);
  }
  void loop(GameWindow &window_, EventBase event_) {
    shape.setPosition(posInWin.x, posInWin.y);
    shape.setPoint(0, {0, 0});
    shape.setPoint(1, {size.x, 0});
    shape.setPoint(2, {size.x, size.y});
    shape.setPoint(3, {0, size.y});
    window_.draw(shape);
  }
};
class FlagCenter {
public:
  sf::ConvexShape shape;
  FlagCenter() {
    shape.setPointCount(3);
    shape.setPoint(0, {0, 0});
    shape.setPoint(1, {4, 4});
    shape.setPoint(2, {-4, 4});
    shape.setFillColor(sf::Color(255, 0, 0));
  }

  void draw(GameWindow &window_, const IVector2 &posWin_) {
    shape.setPosition(posWin_.x, posWin_.y);
    window_.draw(shape);
  }
};
//////////////
class WorldForEditor : public GWorld {
public:
  MovableAxis axis;
  RectForEditor rectForMouseSelect;
  FlagCenter shapeOfCenter;
  std::set<GActor *> actorsSelected;
  enum { nothing, selected, selecting } state = nothing;
  WorldForEditor() {}
  void bindInput() {
    getControllerActive()->bind(GController::q, [this]() { saveWorldData(); });
    getControllerActive()->bind(GController::custom, [this]() {
      if (state == selected) {
        axis.selected = true;
        axis.listenEvent(gm.gameIns->window, gm.gameIns->event);
        if (axis.activeMO) {
          auto deltaMove = axis.getDeltaPosWin() *
                           gm.gameIns->window.getCameraActve()->getPixSize();
          for (GActor *actor : actorsSelected) {
            actor->addPositionOffsetWs({deltaMove.x, deltaMove.y, deltaMove.z});
          }
        }
        if (axis.selected == false) {
          state = nothing;
        }
      } else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        IVector2 posMouse =
            (IVector2)(gm.gameIns->window.getMousePositionWin());
        ;
        if (actorsSelected.empty()) {
          if (state != selecting) {
            rectForMouseSelect.posInWin =
                (IVector2)(gm.gameIns->window.getMousePositionWin());
          }
          state = selecting;
          rectForMouseSelect.size =
              (FVector2)(posMouse - rectForMouseSelect.posInWin);
        }
      } else {
        if (state == selecting) {
          selectActors(rectForMouseSelect);
        }
        if (actorsSelected.empty()) {
          state = nothing;
        } else {
          state = selected;
        }

        rectForMouseSelect.size = {0, 0};
        rectForMouseSelect.posInWin = {-1, -1};
      }
      if (state == nothing) {
        actorsSelected.clear();
      }
    });
  }
  void axisAttachActor() {
    if (actorsSelected.empty()) {
      return;
    }
    const FVector3 &actorPos = (*(actorsSelected.begin()))->getPositionWs();
    const IVector2 &postemp =
        gm.gameIns->window.wsToWin({actorPos.x, actorPos.y, 0});
    axis.posInWin = {(float)postemp.x, (float)postemp.y};
  }
  void selectActors(RectForEditor &rect) {
    for (auto actor : getGridMap().actorsAlive) {
      const IVector2 posTemp =
          gm.gameIns->window.wsToWin(actor->getPositionWs());

      if (rect.containPos(IVector2(posTemp.x, posTemp.y)) &&
          !(actor->isEditorObj)) {
        actorsSelected.insert(actor);
      }
    }
    if (!actorsSelected.empty()) {
      axis.selected = true;
    }
  }
  void showSelectedActors(GameWindow &window_) {
    for (auto actor : actorsSelected) {
      const IVector2 posTemp =
          gm.gameIns->window.wsToWin(actor->getPositionWs());
      shapeOfCenter.draw(window_, posTemp);
    }
  }
  void tick() override {
    PRINTDEBUG(L"Actors:%d", getGridMap().getActorsNumber());
  }
  void loop(GameWindow &window_, EventBase &event_) override {
    getControllerActive()->loop(window_, event_);
    window_.clear();
    tick();
    landScape.draw(window_);
    pollActorsActive(window_);
    showGridMap(window_);
    if (axis.selected) {
      axis.loop(window_, event_);
      axisAttachActor();
    }
    rectForMouseSelect.loop(window_, event_);
    showSelectedActors(window_);
    GDebug::debugDisplay(window_);

    window_.display();
  }
  void saveWorldData() {
    std::ofstream ofile;
    ofile.open("res/myWorld.json");
    nlohmann::json jsonObj;
    const std::vector<GActor *> &all = getGridMap().getAllActors();

    jsonObj["mapInfo"]["row"] = getGridMap().row;
    jsonObj["mapInfo"]["column"] = getGridMap().column;
    jsonObj["mapInfo"]["width"] = getGridMap().width;
    jsonObj["mapInfo"]["height"] = getGridMap().height;
    for (GActor *actor : all) {
      std::string className;
      try {
        className = actor->getGClass().className;
      } catch (const std::exception &e) {
        continue;
      }
      if (className == "GStaticActor") {

        const FVector3 &pos = actor->getPositionWs();
        const std::string name = ((GStaticActor *)actor)->infoPtr->name;
        const FVector3 &size = ((GStaticActor *)actor)->sprComp->getSizeWs();
        nlohmann::json j;

        j["name"] = name;
        j["position"] = {pos.x, pos.y, pos.z};
        j["sizeWs"] = {size.x, size.y, size.z};
        jsonObj["staticActors"].push_back(j);
      }
      if (className == "GAnimActor") {
        size_t texId = ((GAnimActor *)actor)
                           ->sprComp->getRenderSpr()
                           ->getTexturePtr()
                           ->idAndPath.get_hash();
        std::string path = ((GAnimActor *)actor)
                               ->sprComp->getRenderSpr()
                               ->getTexturePtr()
                               ->idAndPath.getStringStd();
        int beginIndex =
            ((GAnimActor *)actor)->sprComp->getAnimation().getIdBegin();
        int endIndex =
            ((GAnimActor *)actor)->sprComp->getAnimation().getIdEnd();
        int frameSpeed =
            ((GAnimActor *)actor)->sprComp->getAnimation().getFramePerS();
        const FVector3 &pos = actor->getPositionWs();

        const FVector3 &size = ((GAnimActor *)actor)->sprComp->getSizeWs();
        nlohmann::json j;
        j["texId"] = texId;
        j["path"] = path;
        j["beginIndex"] = beginIndex;
        j["endIndex"] = endIndex;
        j["frameSpeed"] = frameSpeed;
        j["position"] = {pos.x, pos.y, pos.z};
        j["sizeWs"] = {size.x, size.y, size.z};
        jsonObj["animActors"].push_back(j);
      }
    }
    // 地形信息
    std::string landScapeShaderPath =
        landScape.getShader().shader->idAndPath.getStringStd();
    jsonObj["landScapeShader"] = landScapeShaderPath;
    // gameMode
    jsonObj["gameMode"]["playerClass"] = gm.player->getGClass().className;
    jsonObj["gameMode"]["playerPosition"] = {gm.player->getPositionWs().x,
                                             gm.player->getPositionWs().y,
                                             gm.player->getPositionWs().z};
    // 保存
    ofile << jsonObj.dump(4);
  }
};

class EditorWindowWithPanel : public GGame {
public:
  BigWindow *UI = nullptr;
  EditorWindowWithPanel() {
    UI = getUiManager().createBigWindow(L"ui", window.getSystemHandle());
  }
  virtual ~EditorWindowWithPanel() {
    // 销毁UI
  }
};
class WorldEditorWindow : public EditorWindowWithPanel {
public:
  WorldEditorWindow() { init(); }
  void init() {
    setUI();
    loadWorld<WorldForEditor>("res/myWorld.json");
    curWorld->setControllerActive(curWorld->getControllerDefault());
    window.setCameraActive(curWorld->getCameraDefault());
    ((WorldForEditor *)curWorld)->bindInput();
  }

  void setUI();
  void loop() {
    while (window.isOpen()) {
      curWorld->loop(window, event);
      getUiManager().MainLoop();
    }
  }

  void runGame() {
    GGame g;
    g.loadWorld<GWorld>("res/myWorld.json");
    g.loop();
  }
  // 窗口方法
};

#endif // WORLDEDITOR_H