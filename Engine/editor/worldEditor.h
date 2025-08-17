#include "framework.h"
#include "render/sprite.h"

class MovableObj {
public:
  GSprite spr;
  IVector2 posPressedRelative = {0, 0};
  MovableObj() {}
  void init(GTexture &tex) { spr.init(tex); }
  void draw(GameWindow &window_, IVector2 &posWIn) {
    spr.setPositionWin(posWIn.x, posWIn.y);
    spr.drawWin(window_);
  }
  bool isContainMouse(GameWindow &window_) {
    const IVector2 &pos = sf::Mouse::getPosition(window_);
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
  IVector2 posInWin = {300, 300};
  float zPosWin = 0;
  float zPosLastTime = -1;
  void setZPosOffset(float offset_) { zPosWin += offset_; }
  MovableAxis() {

    tex[0].init(1, 1, 0.5, 1.3, "system/texture/x.png");
    tex[1].init(1, 1, 0.5, 1.3, "system/texture/y.png");
    tex[2].init(1, 1, 0.5, 1.4246, "system/texture/z.png");
    tex[3].init(1, 1, 0.1, 0.9, "system/texture/c.png");
    axisX.init(tex[0]);
    axisX.spr.setSizeWin(10, 100);
    axisX.spr.setRotation(90);
    axisY.init(tex[1]);
    axisY.spr.setSizeWin(10, 100);
    axisZ.init(tex[2]);
    axisZ.spr.setSizeWin(10, 100);
    axisZ.spr.setRotation(45);
    center.init(tex[3]);
    center.spr.setSizeWin(30, 30);
    axisX.cbk = [this](GameWindow &window_) {
      posInWin.x =
          sf::Mouse::getPosition(window_).x - axisX.posPressedRelative.x;
    };
    axisY.cbk = [this](GameWindow &window_) {
      posInWin.y =
          sf::Mouse::getPosition(window_).y - axisY.posPressedRelative.y;
    };
    axisZ.cbk = [this](GameWindow &window_) {
      float currentMousePos = sf::Mouse::getPosition(window_).y;
      if (zPosLastTime == -1) {
        zPosLastTime = currentMousePos;
        return;
      }
      float deltaMouseMove = currentMousePos - zPosLastTime;
      setZPosOffset(deltaMouseMove);
      zPosLastTime = currentMousePos;
    };
    center.cbk = [this](GameWindow &window_) {
      posInWin = sf::Mouse::getPosition(window_) - center.posPressedRelative;
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
      const IVector2 &posRelative = sf::Mouse::getPosition(window_) - posInWin;
      if (activeMO == nullptr) {
        if (axisX.isContainMouse(window_)) {
          activeMO = &axisX;
          axisX.posPressedRelative = posRelative;
        } else if (axisY.isContainMouse(window_)) {
          activeMO = &axisY;
          axisY.posPressedRelative = posRelative;
        } else if (axisZ.isContainMouse(window_)) {
          activeMO = &axisZ;
          //   axisZ.posPressedRelative = posRelative;
        } else if (center.isContainMouse(window_)) {
          activeMO = &center;
          center.posPressedRelative = posRelative;
        }
      }
    } else {
      activeMO = nullptr;
      zPosLastTime = -1;
    }

    if (activeMO) {
      activeMO->cbk(window_);
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
class FlagCenter{};
//////////////
class WorldForEditor : public GWorld {
public:
  MovableAxis axis;
  RectForEditor rectForMouseSelect;
  std::set<GActor*> actorsSelected;
  enum { nothing, selected, selecting } state = nothing;
  WorldForEditor() {}
  void beginPlay() override {
    getControllerActive()->bind(GController::custom, [this]() {
      if (axis.selected) {
        state=selected;
        axis.listenEvent(gm.gameIns->window, gm.gameIns->event);
      }

      if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        IVector2 posMouse = sf::Mouse::getPosition(gm.gameIns->window);
        if (actorsSelected.empty()) {
          if (state != selecting) {
            rectForMouseSelect.posInWin =
                sf::Mouse::getPosition(gm.gameIns->window);
          }
          state = selecting;
          rectForMouseSelect.size =
              (FVector2)(posMouse - rectForMouseSelect.posInWin);
        }
      } else {
        if (state == selecting) {
          selectActors(rectForMouseSelect);
        }
        
        state = nothing;
        rectForMouseSelect.size = {0, 0};
        rectForMouseSelect.posInWin = {-1, -1};
      }
      if (state == nothing) {
        actorsSelected.clear();
      }
    });
   
    loadBaseActors("res/myWorld.json");
  }
  void selectActors(RectForEditor &rect) {
    const FVector2 &windowSize = gm.gameIns->window.getDefaultView().getSize();
    for (auto actor : getGridMap().actorsAlive) {
      const FVector3 posTemp = getCameraActive()->wsToWin(
          actor->getPositionWs(), windowSize.x, windowSize.y);
      if (rect.containPos(IVector2(posTemp.x, posTemp.y))) {
        actorsSelected.insert(actor);
      }
    }
    if(!actorsSelected.empty()){axis.selected=true;}
    
  }
  void tick() override {
    PRINTDEBUG(L"Actors:%d", getGridMap().getActorsNumber());
  }
  void loop(GameWindow &window_, EventBase &event_) override {
    getControllerActive()->loop(window_, event_);
    window_.clear();
    pollActorsActive(window_);
    showGridMap(window_);
    if (axis.selected) {
      axis.loop(window_, event_);
    }
    rectForMouseSelect.loop(window_, event_);
    GDebug::debugDisplay(window_);

    window_.display();
  }
};
class WorldEditorWindow : public GGame {
public:
  WorldEditorWindow() { createWorld<WorldForEditor>(); }
  void loop() {
    while (window.isOpen()) {
      curWorld->loop(window, event);
    }
  }
};