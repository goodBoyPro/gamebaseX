class GObject {};
class GComponent : GObject {};
class GRenderObj:GComponent{};
class GActor : GObject {};
class GActorComponent : GActor {};
class GCamera : GActorComponent {};
class GPlayer:GActor{};
class GWorld : GObject {};
class GGame : GObject {};
