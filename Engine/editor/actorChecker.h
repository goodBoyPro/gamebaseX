#ifndef ACTORCHECHER_H
#define ACTORCHECHER_H
#include "framework.h"

class ActorChecker : public GGame {
public:
  GActor *currentActor = nullptr;
  ActorChecker() {
    window.close();
    window.create(sf::VideoMode(400,400), "",sf::Style::None,GameStatics::getWindowContexSettings());
    createWorld<GWorld>(2, 2, 600, 600);
  }
  void loop() override { curWorld->loop(window, event); }
};
#endif // ACTORCHECHER_H