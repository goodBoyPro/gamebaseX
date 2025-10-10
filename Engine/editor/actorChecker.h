#ifndef ACTORCHECHER_H
#define ACTORCHECHER_H
#include "framework.h"

class ActorChecker : public GGame {
public:
  GActor *currentActor = nullptr;
  ActorChecker() {
    window.close();
    window.create(sf::VideoMode(400,400), "",sf::Style::None);
    createWorld<GWorld>(2, 2, 600, 600);
    setCurrentActor("GStaticActor", "tree1");
  }
  void setCurrentActor(const std::string &type_, const std::string &name_) {
    delete currentActor;

    if (type_ == "GStaticActor") {
      currentActor = curWorld->createStaticActor(name_);
    } else {
      currentActor = curWorld->createActorByClassName(type_);
    }
  }
  void loop() override { curWorld->loop(window, event); }
};
#endif // ACTORCHECHER_H