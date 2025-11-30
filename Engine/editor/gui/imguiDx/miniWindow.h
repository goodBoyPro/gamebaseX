#ifndef MINIWINDOW_H
#define MINIWINDOW_H
#include "imguiLib/imgui.h"
#include <cstdio>
#include <functional>
#include <string>

#include <Windows.h>
inline float GetMenuBarHeight() { return 17; }
class MiniWindow {
public:
  bool couldClose = true;

  ImVec2 position = {100, 0};
  ImVec2 size = {100, 300};

public:
  class WindowArea *parentArea = nullptr;
  MiniWindow(const std::string &id_, int state_) {
    id = id_;
    state = state_;
    ImGuiStyle &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(0.1, 0.1, 0.1, 1);
  };
  virtual ~MiniWindow() {};
  std::function<void()> imguiRenderCBK = []() {};
  void disableClose() { couldClose = false; }

public:
  std::string id;
  bool bOpen = true;
  void setPosition(const ImVec2 &pos_) { position = pos_; }
  void setSize(const ImVec2 &size_) { size = size_; }
  int state = 0;
  // 这里分开写是因为需要在lamda表达式内部调用自身
  void setWindowUi(const std::function<void()> &callback) {
    imguiRenderCBK = callback;
  };
  bool canMoveResize = true;
  virtual void loop() {
    if ((bOpen)) {
      if (!canMoveResize) {
        ImGui::SetNextWindowPos(position);
        ImGui::SetNextWindowSize(size);
      }
      if (couldClose) {

        if (ImGui::Begin(id.c_str(), &bOpen, state)) {
          imguiRenderCBK();
          ImGui::End();
        };
      } else {
        if (ImGui::Begin(id.c_str(), nullptr, state)) {
          imguiRenderCBK();
          ImGui::End();
        };
      }
    }
  }
};
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
class WindowArea {
public:
  ImVec2 position = {0, 0};
  ImVec2 size = {100, 100};
  void setPosition(const ImVec2 &pos_) { position = pos_; }
  void setSize(const ImVec2 &size_) { size = size_; }
  ImVec2 setPositionSize(float x, float y, float w, float h) {
    position = {x, y};
    size = {w, h};
    return ImVec2(w, h);
  }
  std::vector<MiniWindow *> allWindows;
  MiniWindow *addWindow(MiniWindow *w) {
    w->parentArea = this;
    w->canMoveResize = false;
    allWindows.push_back(w);
    return w;
  }
  void autoMatchPositionSize() {
    if (allWindows.size() == 0) {
      return;
    }
    ImVec2 currentPos = position;

    for (MiniWindow *w : allWindows) {

      w->setPosition(currentPos);
      w->setSize({size.x, size.y / allWindows.size()});
      currentPos.y = currentPos.y + size.y / allWindows.size();
    }
  }
};
inline bool IsPointInRect(const ImVec2 &point, const ImVec2 &rectMin,
                          const ImVec2 &rectMax) {
  return (point.x >= rectMin.x && point.x <= rectMax.x &&
          point.y >= rectMin.y && point.y <= rectMax.y);
}

class Splitter {
private:
  bool isVertical;      // 垂直分割线还是水平分割线
  float splitRatio;     // 分割比例
  float thickness;      // 分割线厚度
  float hoverThickness; // 悬停时的厚度
  ImVec4 color;         // 分割线颜色
  ImVec4 hoverColor;    // 悬停时的颜色
  bool isDragging;      // 是否正在拖动
  float dragStartPos;   // 拖动开始时的位置
  float minRatio;       // 最小比例限制
  float maxRatio;       // 最大比例限制

public:
  // 构造函数
  Splitter(bool vertical = false, float ratio = 0.5f)
      : isVertical(vertical), splitRatio(ratio), thickness(4.0f),
        hoverThickness(6.0f), color(0.3f, 0.3f, 0.3f, 0.6f),
        hoverColor(0.5f, 0.5f, 0.5f, 0.8f), isDragging(false),
        dragStartPos(0.0f), minRatio(0.1f), maxRatio(0.9f) {}

  // 渲染分割线并处理输入
  bool render(const ImVec2 &pos, float length, float LengthTotal) {
    ImGuiIO &io = ImGui::GetIO();
    ImDrawList *drawList = ImGui::GetWindowDrawList();

    // 计算分割线的位置和大小
    ImVec2 start, end;
    if (isVertical) {
      start = ImVec2(pos.x, pos.y);
      end = ImVec2(pos.x, pos.y + length);
    } else {
      start = ImVec2(pos.x, pos.y);
      end = ImVec2(pos.x + length, pos.y);
    }

    // 检查鼠标是否悬停在分割线上
    bool isHovered = false;
    float currentThickness = thickness;
    ImVec4 currentColor = color;

    // 计算分割线的碰撞区域（使用两个点代替ImRect）
    ImVec2 hitBoxMin, hitBoxMax;
    if (isVertical) {
      hitBoxMin = ImVec2(start.x - thickness / 2, start.y);
      hitBoxMax = ImVec2(start.x + thickness / 2, end.y);
    } else {
      hitBoxMin = ImVec2(start.x, start.y - thickness / 2);
      hitBoxMax = ImVec2(end.x, start.y + thickness / 2);
    }

    // 检查鼠标交互
    if (isDragging) {
      // 处理拖动
      float mousePos = isVertical ? io.MousePos.x : io.MousePos.y;
      float delta = mousePos - dragStartPos;
      dragStartPos = mousePos;

      // 计算新的比例（基于总长度）

      splitRatio += delta / LengthTotal;

      // 限制比例范围
      if (splitRatio < minRatio)
        splitRatio = minRatio;
      if (splitRatio > maxRatio)
        splitRatio = maxRatio;

      currentThickness = hoverThickness;
      currentColor = hoverColor;

      // 拖动结束
      if (!ImGui::IsMouseDown(0)) {
        isDragging = false;
        return true; // 拖动完成
      }
    } else {
      // 检查悬停（使用我们自己的IsPointInRect函数）
      isHovered = IsPointInRect(io.MousePos, hitBoxMin, hitBoxMax);

      // 检查鼠标按下
      if (isHovered && ImGui::IsMouseClicked(0)) {
        isDragging = true;
        dragStartPos = isVertical ? io.MousePos.x : io.MousePos.y;
        currentThickness = hoverThickness;
        currentColor = hoverColor;
      }
    }

    // 如果悬停，改变外观
    if (isHovered && !isDragging) {
      currentThickness = hoverThickness;
      currentColor = hoverColor;
      // 改变鼠标光标

      if (isVertical) {
        // 垂直分割线：左右调整光标
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
      } else {
        // 水平分割线：上下调整光标
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
      }
    }

    // 绘制分割线
    drawList->AddLine(start, end, ImGui::GetColorU32(currentColor),
                      currentThickness);

    return isDragging;
  }

  // 获取当前分割比例
  float getRatio() const { return splitRatio; }

  // 设置分割比例
  void setRatio(float ratio) {
    splitRatio = ratio;
    if (splitRatio < minRatio)
      splitRatio = minRatio;
    if (splitRatio > maxRatio)
      splitRatio = maxRatio;
  }

  // 设置比例限制
  void setRatioLimits(float min, float max) {
    minRatio = min;
    maxRatio = max;
    // 确保当前比例在新的限制范围内
    setRatio(splitRatio);
  }

  // 设置分割线方向
  void setVertical(bool vertical) { isVertical = vertical; }

  // 检查是否正在拖动
  bool dragging() const { return isDragging; }
};
class WindowLayOut {
  float menuBarHeight = 21;
  float spilitterThickness = 4;
  float x11 = 0.2;
  float x12 = 0.8;
  float x21 = 0.8;
  float y1 = 0.8;
  Splitter splitterLeft{true, x11};
  Splitter splitterRight{true, x12};
  Splitter splitterBottom{false, y1};
  Splitter splitterBottomRight{true, x21};
  int width = 1;
  int height = 1;

public:
  void setMenuHeight(float h) { menuBarHeight = h; }
  WindowArea areaLeft;
  WindowArea areaCenter;
  WindowArea areaRight;
  WindowArea areaBottom;
  WindowArea areaBottomRgiht;
  WindowLayOut() {}
  void renderloop() {

    // 渲染左侧分割器
    ImGui::SetNextWindowPos({0, menuBarHeight});
    ImGui::SetNextWindowSize({(float)width, (float)(height - menuBarHeight)});
    if (ImGui::Begin("SplitterContainer", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
                         //  ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoBackground | // 不绘制窗口背景
                         //  ImGuiWindowFlags_NoSavedSettings |
                         ImGuiWindowFlags_NoBringToFrontOnFocus //
                     )) {
      if (splitterLeft.render(
              ImVec2(width * splitterLeft.getRatio(), menuBarHeight),
              height * y1, width)) {
        x11 = splitterLeft.getRatio();
        match(GetForegroundWindow());
      };
      if (splitterRight.render(
              ImVec2(width * splitterRight.getRatio(), menuBarHeight),
              height * y1, width)) {
        x12 = splitterRight.getRatio();
        match(GetForegroundWindow());
      };
      if (splitterBottom.render(
              ImVec2(0, menuBarHeight + height * splitterBottom.getRatio()),
              width, height - menuBarHeight)) {
        y1 = splitterBottom.getRatio();
        match(GetForegroundWindow());
      };
      if (splitterBottomRight.render({width * splitterBottomRight.getRatio(),
                                      height * y1 + menuBarHeight},
                                     height * (1 - y1) + 100, width)) {
        x21 = splitterBottomRight.getRatio();
        match(GetForegroundWindow());
      }
      ImGui::End();
    }
  }
  void match(HWND hwnd_) {
    RECT rect;
    GetClientRect(hwnd_, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top - menuBarHeight;

    // areaLeft.setPositionSize(0, menuBarHeight, width * x1, height *y1);
    areaLeft.setPositionSize(0, menuBarHeight,
                             width * x11 - spilitterThickness / 2,
                             height * y1 - spilitterThickness / 2);

    // areaCenter.setPositionSize(width * x1+spilitterThickness, menuBarHeight,
    // width * (x2-x1), height *y1);
    areaCenter.setPositionSize(width * x11 + spilitterThickness / 2,
                               menuBarHeight,
                               width * (x12 - x11) - spilitterThickness,
                               height * y1 - spilitterThickness / 2);

    // areaRight.setPositionSize(width * x2+spilitterThickness*2, menuBarHeight,
    // width * (1-x2), height *y1);
    areaRight.setPositionSize(width * x12 + spilitterThickness / 2,
                              menuBarHeight,
                              width * (1 - x12) - spilitterThickness,
                              height * y1 - spilitterThickness / 2);

    // areaBottom.setPositionSize(0, menuBarHeight + height
    // *y1+spilitterThickness, width, height * (1-y1));
    areaBottom.setPositionSize(
        0, menuBarHeight + height * y1 + spilitterThickness / 2, width * x21,
        height * (1 - y1) - spilitterThickness / 2);
    areaBottomRgiht.setPositionSize(
        width * x21, menuBarHeight + height * y1 + spilitterThickness / 2,
        width * (1 - x21), height * (1 - y1) - spilitterThickness / 2);

    areaLeft.autoMatchPositionSize();
    areaCenter.autoMatchPositionSize();
    areaRight.autoMatchPositionSize();
    areaBottom.autoMatchPositionSize();
    areaBottomRgiht.autoMatchPositionSize();
  }
};
#endif // MINIWINDOW_H