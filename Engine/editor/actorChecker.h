#ifndef ACTORCHECHER_H
#define ACTORCHECHER_H
#include "framework.h"
class WorldForActorChecker : public GWorld {
public:
  WorldForActorChecker() {}
  virtual void render(GameWindow &window_, EventBase &event_) override;
  void drawReferLines(GameWindow &window_);
  float lineDistanceWs=1;
  ColorBase lineColor={255,255,0,150};
  ColorBase xLineColor={255,0,0,200};
  ColorBase yLineColor={0,255,0,200};
};
class ActorChecker : public GGame {
public:
  GActor *currentActor = nullptr;
  ActorChecker() {
    window.close();
    window.create(sf::VideoMode(400, 400), "", sf::Style::None,
                  GameStatics::getWindowContexSettings());
    createWorld<WorldForActorChecker>(2, 2, 600, 600);
  }
  void loop() override { curWorld->loop(window, event); }
};
#endif // ACTORCHECHER_H