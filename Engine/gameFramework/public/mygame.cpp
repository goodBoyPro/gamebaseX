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
    
    tex.init(20, 20, 0.5, 1, "res/comb.png");
    tex2.init(1, 1, 0.5, 0.5, "res/a.png");
    for (int i = 0; i < 10000; i++) {
      auto a = createActor<MyActor>();
      a->construct(tex2, 0);
      a->direction = {(float)sin(i), (float)cos(i), 0};
    }

    createActor<GAnimActor>()->construct(tex, 0, 29, 25);
    setGameMode<MyPlayer>().player->moveComp->speed = 100;
  }
};
int main() {
  GGame *gameins = GGame::getGameIns();
  gameins->createWorld<MyWorld>();
  gameins->loop();
};