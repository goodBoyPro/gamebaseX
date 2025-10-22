#ifndef REFERLINE
#define REFERLINE
#include "base/base.h"
class ReferLine {
public:
  float lineDistanceWs = 1;
  ColorBase lineColor = {255, 255, 0, 150};
  ColorBase xLineColor = {255, 0, 0, 200};
  ColorBase yLineColor = {0, 255, 0, 200};
  ColorBase numberColor = {255, 255, 255, 255};
  int numberSize = 10;
  bool isActive = false;
  ReferLine()=default;
  void drawLines(GameWindow &window_);
  void setActive();
  void disableActive();
};
#endif // REFERLINE