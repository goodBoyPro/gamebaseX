#include <heads.h>
#include "controller.h"
#include "render/sprite.h"
#include "timeManager.h"
#include "iostream"
int mainx() {
    // 创建一个 800x600 像素的窗口，标题为 "SFML Window"
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Window");

    // 设置垂直同步，避免画面撕裂
    window.setVerticalSyncEnabled(true);
    GController controller;
    int handle = controller.bind(GController::a, []() { printf("hello"); });
    controller.bind(GController::q, [&]() { controller.unBind(GController::a, handle); });
    GTexture tex;
    tex.loadFromFile("res/test.png");
   
    GSprite spr(tex,1,1);
    spr.setId(2);
    spr.setSizeWin(500, 500);
    spr.setPositionWin(0,0);
    spr.setCenter(-0.4,-0.4);
    TimeManager timamanager;
    uint64_t tt1 = 0;

    for (int i = 0; i < 1000000; i++) {
        timamanager.addTask(100, 1, [&]() { tt1++; });
    }
    timamanager.addTask(2000, 1, [&]() {std::cout<<tt1;});
        
       
    
    // 主循环
    while (window.isOpen()) {
        // 事件处理
        sf::Event event;
        controller.loop(window, event);

        // 清空窗口，使用黑色背景
        window.clear(sf::Color::Black);

        spr.drawWin(window);
        window.display();
        timamanager.loop();
    }

    return 0;
}