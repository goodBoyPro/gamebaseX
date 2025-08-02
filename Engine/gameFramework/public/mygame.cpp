#include <framework.h>
#include "GDebug.h"
class MyActor : public GStaticActor {
  public:
    void loop(float deltatime_, WindowBase &window_) override {
        GStaticActor::loop(deltatime_, window_);
        PRINTDEBUG(L"test%d",10);
        
    }
};
class MyWorld : public GWorld {
  public:
    GTexture tex   ;
    MyWorld() {
        tex.init(20, 20, 0.5, 1, "res/comb.png");
        createActor<GStaticActor>()->construct(tex, 0);
        createActor<MyActor>()->construct(tex, 0);
        createActor<GAnimActor>()->construct(tex, 0, 29);
        setGameMode<GPlayer>().player->moveComp->speed=10;
        
    }
};
int main() {
    GGame *gameins = GGame::getGameIns();
    gameins->createWorld<MyWorld>();
    gameins->loop();
};