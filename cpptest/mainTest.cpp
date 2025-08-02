#include"stdio.h"
#include"fstream"
#include "string"
#include"iostream"
#include"base/saveObject.h"
#include"render/gsource.h"

int main(){
  
    GSource gs;
    sf::RenderWindow window(sf::VideoMode(800, 600), "My SFML Window");
    sf::Sprite spr;
    spr.setTexture(gs.getTexture(110110).texture);
    // 主循环（保持窗口运行直到用户关闭）
    while (window.isOpen())
    {
        // 事件处理
        sf::Event event;
        while (window.pollEvent(event))
        {
            // 检测到关闭事件时退出
            if (event.type == sf::Event::Closed)
                window.close();
        }
 
        // 清空窗口（使用绿色填充背景）
        window.clear(sf::Color::Black);
        window.draw(spr);
        // 在此处添加绘制代码...
        
        // 显示绘制内容
        window.display();
    }
 
    return 0;
}