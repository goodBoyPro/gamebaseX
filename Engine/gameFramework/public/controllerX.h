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
    enum Input { a, w, s, d, q,left, count };
    std::vector<std::vector<Cbk>> cbks;
    GController() {
        for (int i = 0; i < (int)count; i++) {
            cbks.push_back(std::vector<Cbk>());
        }
    }
    int bind(Input eInput, const std::function<void()> &cbk) {
        cbkId++;
        cbks[eInput].emplace_back(cbk, cbkId);
        return cbkId;
    }
    void unBind(Input eInput, int handle) {
        auto &vec = cbks[eInput];
        vec.erase(std::remove_if(vec.begin(), vec.end(), [handle](const Cbk &cbk) { return cbk.id == handle; }), vec.end());
    }
    void executeCbk(Input eInput) {
        std::vector<Cbk> &cbks_ = cbks[eInput];
        for (Cbk &cbk : cbks_) {
            cbk.function();
        }
    }
    void loop(sf::RenderWindow &window, sf::Event &event) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            executeCbk(a);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            executeCbk(w);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            executeCbk(s);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            executeCbk(d);
        }
        // 点击时
        while (window.pollEvent(event)) {
            // 关闭窗口事件
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Q) {
                    for (Cbk &cbk : cbks[q]) {
                        cbk.function();
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    for (Cbk &cbk : cbks[left]) {
                        cbk.function();
                    }
                }
            }
        }
    }
};
#endif
