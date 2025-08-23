#include "base.h"
#include "framework.h"
IVector2 GameWindow::wsToWin(const FVector3 &posWs_) {
  
  return cameraActive->wsToWin(posWs_, getDefaultView().getSize().x,
                               getDefaultView().getSize().y);
}
FVector3 GameWindow::winToWs(FVector2 posWin_) {
  return cameraActive->winToWs(posWin_, *this);
}
FVector3 GameWindow::getMousePositionWs() {
  return cameraActive->getMousePositionWs(*this);
}
FVector2 GameWindow::getMousePositionWin() {
  const IVector2 &posWin = sf::Mouse::getPosition(*this);
  return {(posWin.x * this->getDefaultView().getSize().x / this->getSize().x),
          (posWin.y * this->getDefaultView().getSize().y / this->getSize().y)};
};