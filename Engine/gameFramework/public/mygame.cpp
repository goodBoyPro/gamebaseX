#include "GDebug.h"
#include <framework.h>

class MyActor : public GStaticActor {
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
REGISTER_CLASS(MyPlayer)
class MyPlayer : public GPlayer {
  GPrimitiveComponent *primComp = nullptr;
REGISTER_BODY(MyPlayer)
public:
  void beginPlay() override {
    GPlayer::beginPlay();
    setPositionWs({-120,-155,0});
    primComp = createComponent<GPrimitiveComponent>();
  }
  void tick() override {
    PRINTDEBUG(L"node:%d", nodeId);
    PRINTDEBUG(L"window:%d,%d", GGame::getGameIns()->window.getSize().x,
               GGame::getGameIns()->window.getSize().y)
    PRINTDEBUG(L"windowDV:%f,%f",
               GGame::getGameIns()->window.getDefaultView().getSize().x,
               GGame::getGameIns()->window.getDefaultView().getSize().y)
  }
};
class MyWorld : public GWorld {
public:
  MyWorld() { loadBaseActors("res/myWorld.json"); }
  void beginPlay() override {}
  void tick() override {
    GWorld::tick();
    const FVector3& pos=gm.gameIns->window.getCameraActve()->getMousePositionWs(gm.gameIns->window);
    PRINTDEBUG(L"mousePos:%f,%f",pos.x,pos.y)
  }
};
int main() {
  GGame *gameins = GGame::getGameIns();
  GWorld *world = gameins->createWorld<MyWorld>();
  world->setGameMode("MyPlayer").player->moveComp->speed = 1;
  

  gameins->loop();
};