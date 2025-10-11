#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "heads.h"

class GController {
private:
  int cbkId = 0;
  struct Cbk {
    Cbk(const std::function<void()> cbk, int handle) {
      function = cbk;
      id = handle;
    }
    std::function<void()> function;
    int id = 0;
  };

public:
  enum Input {
    a,
    w,
    s,
    d,
    q,
    kup,
    kdown,
    kleft,
    kright,
    mleft,
    mright,
    custom,
    count
  };
  std::map<int, int> inputMap = {
      {a, sf::Keyboard::A}, //
      {w, sf::Keyboard::W},          {s, sf::Keyboard::S},
      {d, sf::Keyboard::D},          {q, sf::Keyboard::Q},
      {kup, sf::Keyboard::Up},       {kdown, sf::Keyboard::Down},
      {kright, sf::Keyboard::Right}, {kleft, sf::Keyboard::Left},{mleft,sf::Mouse::Left}};
  std::vector<int> inputLinearV;
  std::vector<int> inputLinearK;
  std::vector<int> inputClickV;
  std::vector<int> inputClickK;

  void setInputLinear() {
    for (auto &in : inputMap) {
      inputLinearK.push_back(in.first);
      inputLinearV.push_back(in.second);
    }
  }
  void setInputClick() {
    for (auto &in : inputMap) {
      inputClickK.push_back(in.first);
      inputClickV.push_back(in.second);
    }
  }

  std::vector<std::vector<Cbk>> linearCbks;
  std::vector<std::vector<Cbk>> clickCbks;
  GController() {
    for (int i = 0; i < (int)count; i++) {
      linearCbks.push_back(std::vector<Cbk>());
    }
    
    for (int i = 0; i < (int)count; i++) {
      clickCbks.push_back(std::vector<Cbk>());
    }
    setInputLinear();
    setInputClick();
  }
  //////////////
  int bindLinearInput(Input eInput, const std::function<void()> &cbk) {
    cbkId++;
    linearCbks[eInput].emplace_back(cbk, cbkId);
    return cbkId;
  }
  void unBindLinearInput(Input eInput, int handle) {
    auto &vec = linearCbks[eInput];
    vec.erase(
        std::remove_if(vec.begin(), vec.end(),
                       [handle](const Cbk &cbk) { return cbk.id == handle; }),
        vec.end());
  }
  void executeLinearCbk(Input eInput) {
    std::vector<Cbk> &cbks_ = linearCbks[eInput];
    for (Cbk &cbk : cbks_) {
      cbk.function();
    }
  }
  //////////////////
  int bindClickInput(Input eInput, const std::function<void()> &cbk) {
    cbkId++;
    clickCbks[eInput].emplace_back(cbk, cbkId);
    return cbkId;
  }
  void unBindClickInput(Input eInput, int handle) {
    auto &vec = clickCbks[eInput];
    vec.erase(
        std::remove_if(vec.begin(), vec.end(),
                       [handle](const Cbk &cbk) { return cbk.id == handle; }),
        vec.end());
  }
  void executeClickCbk(Input eInput) {
    std::vector<Cbk> &cbks_ = clickCbks[eInput];
    for (Cbk &cbk : cbks_) {
      cbk.function();
    }
  }
  //////////////////
  bool isFocused = true;
  void loop(sf::RenderWindow &window, sf::Event &event) {

    // 点击时
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::LostFocus) {
        isFocused = false;
      }
      if (event.type == sf::Event::GainedFocus) {
        isFocused = true;
      }
      if (!isFocused) {
        return;
      }
      // 关闭窗口事件
      if (event.type == sf::Event::Closed)
        window.close();
      //点击按键
      if (event.type == sf::Event::KeyPressed ||
          event.type == sf::Event::MouseButtonPressed) {
        int i = 0;
        for (int in : inputClickV) {
          if (event.key.code == (sf::Keyboard::Key)in ||
              event.mouseButton.button == (sf::Mouse::Button)in) {//暂时这样，实际会冲突，需要修改
            executeClickCbk((Input)inputClickK[i]);
          }

          i++;
        }
      }

      for (Cbk &cbk : clickCbks[custom]) {
        cbk.function();
      }
    }
    // 线性按键
    if (!isFocused) {
      return;
    }
    int i = 0;
    for (int in : inputLinearV) {
      if (sf::Keyboard::isKeyPressed((sf::Keyboard::Key)in)) {
        executeLinearCbk((Input)inputLinearK[i]);
      }
      i++;
    }
    for (Cbk &cbk : linearCbks[custom]) {
      cbk.function();
    }
  }
};
#endif
