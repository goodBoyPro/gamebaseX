#include "referLine.h"
#include "framework.h"
void ReferLine::drawLines(GameWindow &window_) {
  if (!isActive)
    return;
  IVector2 origin = window_.wsToWin({0, 0, 0});
  float pixSize = window_.getCameraActve()->getPixSize();
  float distanceWin = lineDistanceWs / pixSize;
  float WindowW = window_.getDefaultView().getSize().x;
  float windowH = window_.getDefaultView().getSize().y;
  static std::vector<FVector2> points = {{
                                             0,
                                             0,
                                         },
                                         {0, 0}};
  bool a = true;
  bool b = true;
  bool c = true;
  bool d = true;
  float v = 0;
  for (float x = origin.x; x < WindowW; x += distanceWin) {
    points[0].x = x;
    points[0].y = 0;
    points[1].x = x;
    points[1].y = windowH;
    if (a) {
      window_.getCameraActve()->drawLineWin(points, window_, xLineColor);
      a = false;
    } else {
      window_.getCameraActve()->drawLineWin(points, window_, lineColor);
    }
    printTextFormat(window_, x, 0, numberSize, numberColor, 0, L"%.2fm", v);
    v++;
  }
  v = 0;
  for (float x = origin.x; x > 0; x -= distanceWin) {
    points[0].x = x;
    points[0].y = 0;
    points[1].x = x;
    points[1].y = windowH;
    if (b) {
      window_.getCameraActve()->drawLineWin(points, window_, xLineColor);
      b = false;
    } else {
      window_.getCameraActve()->drawLineWin(points, window_, lineColor);
    }
    printTextFormat(window_, x, 0, numberSize, numberColor, 0, L"%.2fm", v);
    v--;
  }
  v = 0;
  for (float y = origin.y; y < windowH; y += distanceWin) {
    points[0].x = 0;
    points[0].y = y;
    points[1].x = WindowW;
    points[1].y = y;
    if (c) {
      window_.getCameraActve()->drawLineWin(points, window_, yLineColor);
      c = false;
    } else {
      window_.getCameraActve()->drawLineWin(points, window_, lineColor);
    }
    printTextFormat(window_, 0, y, numberSize, numberColor, 0, L"%.2fm", v);
    v++;
  }
  v = 0;
  for (float y = origin.y; y > 0; y -= distanceWin) {
    points[0].x = 0;
    points[0].y = y;
    points[1].x = WindowW;
    points[1].y = y;
    if (d) {
      window_.getCameraActve()->drawLineWin(points, window_, yLineColor);
      d = false;
    } else {
      window_.getCameraActve()->drawLineWin(points, window_, lineColor);
    }
    printTextFormat(window_, 0, y, numberSize, numberColor, 0, L"%.2fm", v);
    v--;
  }
};
void ReferLine::setActive() { isActive = true; }
void ReferLine::disableActive() { isActive = false; }
