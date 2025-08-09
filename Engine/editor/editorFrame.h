#ifndef EDITORFRAME_H
#define EDITORFRAME_H
#include <Windows.h>
#include <base/base.h>
#include <string.h>
#include <vector>

class EditorWindow {

  std::vector<EditorWindow *> allChildWindow;
  IVector2 relativeLocation = {6,0};
  IVector2 posLast;
  IVector2 childPosLast;
  bool isActive = true;

public:
  GameWindow window;
  void setACtive() { isActive = true; }
  void disableActive() { isActive = false; }
  template <class T>
  EditorWindow *createEditorWindow(int x, int y,
                                   const std::string &name = "window",
                                   UINT32 style = sf::Style::Default) {
    EditorWindow *windowTemp = new T;
    windowTemp->construct(x, y, name, style);
    windowTemp->relativeLocation.y = window.getSize().y+32;
    
    windowTemp->hideToolWindow();
   
    allChildWindow.push_back(windowTemp);
    return windowTemp;
  }
  EditorWindow() {}
  virtual void begin(){}
  void construct(int x, int y, const std::string &name = "window",
                 UINT32 style = sf::Style::Default) {

    window.create(sf::VideoMode(x, y), name, style, sf::ContextSettings());
    window.setKeyRepeatEnabled(false);
    begin();
  }
  void hideToolWindow() {
    HWND hwnd = window.getSystemHandle();
    DWORD dwExStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    dwExStyle |= WS_EX_TOOLWINDOW;
    dwExStyle &= ~WS_EX_APPWINDOW;
    SetWindowLong(hwnd, GWL_EXSTYLE, dwExStyle);
  }
  virtual ~EditorWindow() {
    for (EditorWindow *childWindow : allChildWindow) {
      delete childWindow;
    }
  }
  virtual void loop(sf::Event &event_) {
    if (window.isOpen()) {
      const IVector2 posNew = window.getPosition();

      if (posLast != posNew) {

        for (EditorWindow *childWindow : allChildWindow) {
          if (childWindow->window.isOpen()) {

            IVector2 newChildPos = posNew + childWindow->relativeLocation;
            childWindow->window.setPosition(newChildPos);

            childWindow->childPosLast = newChildPos;
          }
        }
        posLast = posNew;
      } else {

        for (EditorWindow *childWindow : allChildWindow) {
          IVector2 childPosNew = childWindow->window.getPosition();

          if (childPosNew != childWindow->childPosLast) {

            childWindow->relativeLocation = childPosNew - posNew;

            childWindow->childPosLast = childPosNew;
          }
        }
      }
      if (window.pollEvent(event_)) {
        if (event_.type == sf::Event::Closed) {
          window.close();
        }
      }
      for (EditorWindow *childWindow : allChildWindow) {
        childWindow->loop(event_);
      }
      window.clear();
      window.display();
    }
  }

};

class GEditor : public EditorWindow {
  sf::Event event;

public:
  GEditor() {}
  void begin() override{EditorWindow *winChild = createEditorWindow<EditorWindow>(
    300, 300, "win", sf::Style::None);}
  void mainloop() {
    while (window.isOpen()) {
      loop(event);
    }
  }
  ~GEditor() {}
};

#endif // EDITORFRAME_H