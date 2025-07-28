#include <framework.h>
class MyWorld : public GWorld {
  public:
    GTexture tex   ;
    MyWorld() {
        tex.init(20, 20, 0.5, 1, "res/comb.png");
        createActor<GStaticActor>()->construct(tex, 0);
        createActor<GAnimActor>()->construct(tex, 0, 29);
        setGameMode<GPlayer>();
    }
};
int main() {
    GGame *gameins = GGame::getGameIns();
    gameins->createWorld<MyWorld>();
    gameins->loop();
};