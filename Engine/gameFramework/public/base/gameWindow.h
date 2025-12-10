#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H
#include"base.h"
class GameWindow : public WindowBase {
  class GCameraObj *cameraActive = nullptr;

public:
  void setCameraActive(GCameraObj *cameraComp_) {
    cameraActive = cameraComp_;
  }
  GCameraObj *getCameraActve() { return cameraActive; }
  IVector2 wsToWin(const FVector3 &posWs_);
  FVector3 winToWs(FVector2 posWin_);
  FVector3 getMousePositionWs();
  FVector2 getMousePositionWin();
};
#endif//GAMEWINDOW_H