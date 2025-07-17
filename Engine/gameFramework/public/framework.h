#include "heads.h"
class GObject {};
class GComponent : GObject {};
class GRenderObj : GComponent {};
class GActor : GObject {};
class GActorComponent : GActor {};
class GCamera : GActorComponent {};
class GPlayer : GActor {};
class GWorld : GObject {
   private:
    struct ActorsType{};
    std::vector<GActor *> allActors;

   public:
};
class GGame : GObject {};
