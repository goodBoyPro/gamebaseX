#ifndef FRAMEWORK_H 
#define FRAMEWORK_H
#include "heads.h"
class GObject {
   public:
    ~GObject() {};
};
class GComponent : public GObject {
  public:
    virtual void loop() {};
};
class GRenderObj : public GComponent {};
class GActor : GObject {
  private:
    std::vector<GComponent *> __allComponents;
    std::vector<class GActorComponent *> __allActorComponents;
    FVector3 positionWs = {0, 0, 0};

  public:
    virtual void loop(float deltatime, GameWindow &window);
    const FVector3 &getPositionWs() const { return positionWs; }
    void setPositionWs(const FVector3 &posWs_) { positionWs = posWs_; };
    GComponent*createComponent();
    GActorComponent*createActorComponent();
};
class GActorComponent : public GActor {
  private:
    GActor *parent = nullptr;
    FVector3 positionRelative = {0, 0, 0};
    void followParent() { setPositionWs(parent->getPositionWs() + positionRelative); }

  public:
  GActorComponent(){}
    const FVector3 &getPositionRelative() const { return positionRelative; }
    void setPositionRelative(const FVector3 &posRelative) { positionRelative = posRelative; }
    virtual void loop(float deltatime, GameWindow &window) override { followParent(); }
};

class GCamera : public GActorComponent {};
class GPlayer : public GActor {
   public:
   
};
class GWorld : public GObject {
  private:
    struct ActorsType {};
    std::vector<GActor *> allActors;

  public:
};
class GGame : GObject {};
#endif //FRAMEWORK_H