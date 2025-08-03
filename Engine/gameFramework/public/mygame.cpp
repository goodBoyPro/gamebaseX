#include "GDebug.h"
#include <framework.h>
#include <nlohmann_json/json.hpp>
#include<fstream>
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
  void loadStaticActors() {
    nlohmann::json jsobj;
    std::ifstream ifile("res/myWorld.json");
    ifile >> jsobj;
    ifile.close();
    //staticActor
    for (auto info : jsobj["staticActors"]) {
      int texId = info["texId"];
      int index = info["index"];
      const std::vector<float> &pos =
          info["position"].get<std::vector<float>>();
      FVector3 position = {pos[0], pos[1], pos[2]};
      auto actor = createActor<GStaticActor>(position);
      actor->construct(getSource()->getTexture(texId), index);
    }
    // animActor
    for (auto info : jsobj["animActors"]) {
      int texId = info["texId"];
      int beginIndex = info["beginIndex"];
      int endIndex = info["endIndex"];
      int frameSpeed=info["frameSpeed"];
      const std::vector<float> &pos =
          info["position"].get<std::vector<float>>();
      FVector3 position = {pos[0], pos[1], pos[2]};
      auto actor = createActor<GAnimActor>(position);
      actor->construct(getSource()->getTexture(texId),beginIndex,endIndex,frameSpeed);
    }
   
    
    
  }
  MyWorld() {
    loadStaticActors();
    tex.init(20, 20, 0.5, 1, "res/comb.png");
    tex2.init(1, 1, 0.5, 0.5, "res/a.png");
    

   
    setGameMode<MyPlayer>().player->moveComp->speed = 100;
  }
};
int main() {
  GGame *gameins = GGame::getGameIns();
  gameins->createWorld<MyWorld>();
  gameins->loop();
};