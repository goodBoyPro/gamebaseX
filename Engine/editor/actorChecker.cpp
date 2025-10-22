#include "actorChecker.h"
void WorldForActorChecker::render(GameWindow &window_, EventBase &event_) {
  // 渲染地图
  landScape.draw(window_);

  // actor逻辑
  pollActorsActive(window_);
  // 绘制参考线
  drawReferLines(window_);
  // 世界tick
  tick();
  // UI逻辑
  //
//   showGridMap(window_);
}
void WorldForActorChecker::drawReferLines(GameWindow &window_) {
  IVector2 origin = gm.gameIns->window.wsToWin({0, 0, 0});
  float pixSize = window_.getCameraActve()->getPixSize();
  float distanceWin = lineDistanceWs / pixSize;
  float WindowW = window_.getDefaultView().getSize().x;
  float windowH = window_.getDefaultView().getSize().y;
  static std::vector<FVector2> points={{0,0,},{0,0}};
  bool a = true;
  bool b = true;
  bool c = true;
  bool d = true;
  for (float x = origin.x; x < WindowW; x += distanceWin) {
    points[0].x=x;
    points[0].y=0;
    points[1].x=x;
    points[1].y=windowH;
    if (a) {
      window_.getCameraActve()->drawLineWin(points, window_, xLineColor);
      a = false;
    } else {
      window_.getCameraActve()->drawLineWin(points, window_, lineColor);
    }
  }
  for (float x = origin.x; x > 0; x -= distanceWin) {
    points[0].x=x;
    points[0].y=0;
    points[1].x=x;
    points[1].y=windowH;
     if (b) {
      window_.getCameraActve()->drawLineWin(points, window_, xLineColor);
      b= false;
    } else {
      window_.getCameraActve()->drawLineWin(points, window_, lineColor);
    }
  }
  for (float y = origin.y; y < windowH; y += distanceWin) {
    points[0].x=0;
    points[0].y=y;
    points[1].x=WindowW;
    points[1].y=y;
    if (c) {
      window_.getCameraActve()->drawLineWin(points, window_, yLineColor);
      c = false;
    } else {
      window_.getCameraActve()->drawLineWin(points, window_, lineColor);
    }
  }
  for (float y = origin.y; y > 0; y -= distanceWin) {
    points[0].x=0;
    points[0].y=y;
    points[1].x=WindowW;
    points[1].y=y;
    if (d) {
      window_.getCameraActve()->drawLineWin(points, window_, yLineColor);
      d = false;
    } else {
      window_.getCameraActve()->drawLineWin(points, window_, lineColor);
    }
  }
}
