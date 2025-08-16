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
  GTexture tex[4];
  MovableObj *activeMO = nullptr;
  MovableObj axisX;
  MovableObj axisY;
  MovableObj axisZ;
  MovableObj center;
  IVector2 posInWin = {300, 300};
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
      posInWin.y =
          sf::Mouse::getPosition(window_).y - axisZ.posPressedRelative.y;
    };
    center.cbk = [this](GameWindow &window_) {
      posInWin = sf::Mouse::getPosition(window_) - center.posPressedRelative;
    };
  }
  void loop(GameWindow &window_, EventBase &event_) {

    axisX.draw(window_, posInWin);
    axisY.draw(window_, posInWin);
    axisZ.draw(window_, posInWin);
    center.draw(window_, posInWin);
  }
  void listenEvent(GameWindow &window_, EventBase &event_) {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      const IVector2& posRelative = sf::Mouse::getPosition(window_) - posInWin;
      if (activeMO == nullptr) {
        if (axisX.isContainMouse(window_)) {
          activeMO = &axisX;
          axisX.posPressedRelative = posRelative;
        } else if (axisY.isContainMouse(window_)) {
          activeMO = &axisY;
          axisY.posPressedRelative = posRelative;
        } else if (axisZ.isContainMouse(window_)) {
          activeMO = &axisZ;
          axisZ.posPressedRelative = posRelative;
        } else if (center.isContainMouse(window_)) {
          activeMO = &center;
          center.posPressedRelative = posRelative;
        } else {
          activeMO = nullptr;
        }
      }
    }else{activeMO = nullptr;}

    if (activeMO) {
      activeMO->cbk(window_);
    }
  }
};

class WorldForWorldEditor : public GWorld {
public:
  MovableAxis axis;
  WorldForWorldEditor() {}
  void loop(GameWindow &window_, EventBase &event_) override {
    axis.listenEvent(window_, event_);
    while (window_.pollEvent(event_)) {
      if (event_.type == sf::Event::Closed)
        window_.close();
    }
    window_.clear();
    axis.loop(window_, event_);
    window_.display();
  }
};
class WorldEditorWindow : public GGame {
public:
  WorldEditorWindow() { createWorld<WorldForWorldEditor>(); }
  void loop() {
    while (window.isOpen()) {
      curWorld->loop(window, event);
    }
  }
};