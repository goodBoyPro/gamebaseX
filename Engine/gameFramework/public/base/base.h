#ifndef BASE_H
#define BASE_H
#include <SFML/Graphics.hpp>
typedef sf::Vector3f FVector3;
typedef sf::Vector2f FVector2;
typedef sf::Vector2i IVector2;
typedef sf::RenderWindow WindowBase;
typedef sf::Event EventBase;
typedef sf::Sprite SpriteBase;
typedef sf::Texture TextureBase;
typedef sf::Color ColorBase;
class GameWindow : public WindowBase {
  class GCameraComponent *cameraActive = nullptr;

public:
  void setCameraActive(GCameraComponent *cameraComp_) {
    cameraActive = cameraComp_;
  }
  GCameraComponent*getCameraActve(){return cameraActive;}
};

#endif // BASE_H