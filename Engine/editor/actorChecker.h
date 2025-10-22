#ifndef ACTORCHECHER_H
#define ACTORCHECHER_H
#include "framework.h"
#include"referLine.h"
class WorldForActorChecker : public GWorld {
public:
  WorldForActorChecker() {referLine.setActive();}
  virtual void render(GameWindow &window_, EventBase &event_) override;
  float lineDistanceWs=1;
  ColorBase lineColor={255,255,0,150};
  ColorBase xLineColor={255,0,0,200};
  ColorBase yLineColor={0,255,0,200};
  ColorBase numberColor={255,255,255,255};
  int numberSize=10;
  ReferLine referLine;
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