#include "GDebug.h"
#include <framework.h>
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
REGISTER_CLASS(MyPlayer)
class MyPlayer : public GPlayer {
  
REGISTER_BODY(MyPlayer)
public:
  void beginPlay() override {
    GPlayer::beginPlay();
    setPositionWs({-120,-155,0});
    
  }
  void tick() override {
    PRINTDEBUG(L"node:%d", nodeId);
    PRINTDEBUG(L"window:%d,%d", getWorld()->gm.gameIns->window.getSize().x,
    getWorld()->gm.gameIns->window.getSize().y)
    PRINTDEBUG(L"windowDV:%f,%f",
      getWorld()->gm.gameIns->window.getDefaultView().getSize().x,
      getWorld()->gm.gameIns->window.getDefaultView().getSize().y)
  }
};
class MyWorld : public GWorld {
public:
  MyWorld() {  }
  void beginPlay() override {}
  void tick() override {
    GWorld::tick();
    const FVector3& pos=gm.gameIns->window.getCameraActve()->getMousePositionWs(gm.gameIns->window);
    PRINTDEBUG(L"mousePos:%f,%f",pos.x,pos.y)
  }
};
int main() {
  
  // GGame g;
  // GWorld *world = g.loadWorld<GWorld>("res/myWorld.json");
  
  

  // g.loop();
   GGame g;
      g.loadWorld<GWorld>("temp/mapTemp.json");
      g.loop();
};