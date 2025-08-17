#include "base.h"
#include"framework.h"
IVector2 GameWindow::wsToWin(const FVector3 &posWs_) {
  return cameraActive->wsToWin(posWs_, getDefaultView().getSize().x,getDefaultView().getSize().y);
}
FVector3 GameWindow::winToWs(FVector2 posWin_) {
  return cameraActive->winToWs(posWin_, *this);
}
