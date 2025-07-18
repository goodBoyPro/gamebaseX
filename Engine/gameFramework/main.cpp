#include <heads.h>
#include "controller.h"
int main() {
    // 创建一个 800x600 像素的窗口，标题为 "SFML Window"
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window");

    // 设置垂直同步，避免画面撕裂
    window.setVerticalSyncEnabled(true);
    GController controller;
    int handle = controller.bind(GController::a, []() { printf("hello"); });
    controller.bind(GController::q,
                    [&]() { controller.unBind(GController::a, handle); });

    // 主循环
    while (window.isOpen()) {
        // 事件处理
        sf::Event event;
        controller.loop(window, event);
        

        // 清空窗口，使用黑色背景
        window.clear(sf::Color::Black);

        // 在这里可以绘制图形...

        // 显示窗口内容
        window.display();
    }

    return 0;
}