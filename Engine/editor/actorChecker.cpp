#include "actorChecker.h"
void WorldForActorChecker::render(GameWindow &window_, EventBase &event_) {
  // 渲染地图
  landScape.draw(window_);

  // actor逻辑
  pollActorsActive(window_);
  // 绘制参考线
  referLine.drawLines(window_);
  // 世界tick
  tick();
  // UI逻辑
  //
//   showGridMap(window_);
}
