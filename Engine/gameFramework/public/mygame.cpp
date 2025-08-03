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
class MyPlayer : public GPlayer {
public:
  void tick() override { PRINTDEBUG(L"node:%d", nodeId); }
};
class MyWorld : public GWorld {
public:
  GTexture tex;
  GTexture tex2;
  
  MyWorld() {
    loadBaseActors("res/myWorld.json");
    setGameMode<MyPlayer>().player->moveComp->speed = 100;
  }
};
int main() {
  GGame *gameins = GGame::getGameIns();
  gameins->createWorld<MyWorld>();
  gameins->loop();
};