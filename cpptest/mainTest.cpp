// #include"fileNode.h"
// #include"stdio.h"
// #include"stringCode.h"
// int main(){
//     FileTree ft;
//     ft.loadTree("G:/beifen");
//     for(FileNode* child:ft.root.childs[0]->childs){
//         printf("name:%s\npath:%s\ntype:%d\nexpand:%s\n",utf8_to_gbk(child->name).c_str(),utf8_to_gbk(child->path).c_str(),child->type,child->expand.c_str());
//         printf("-------------------------------------------\n");
//     }
    
//     printf("%s\n",utf8_to_gbk(ft.root.childs[0]->childs[0]->name).c_str());
//     printf("%s\n",wstringToGbk(L"小明").c_str());
    
//     return 0;
// }

#include <SFML/Graphics.hpp>
#include "base/textDrawer.h"
#include "gameStatics.h"
#include <iostream>

int main()
{
    // 1. 创建窗口
    // 参数：窗口大小(宽, 高)、窗口标题、窗口样式（默认带标题栏+关闭按钮）
    sf::RenderWindow window(sf::VideoMode(800, 600), L"SFML 窗口示例", sf::Style::Default);
    
    // 设置窗口帧率上限（避免无意义的高帧率占用资源）
    window.setFramerateLimit(60);
    
    // 2. 主循环（窗口存活期间持续运行）
    while (window.isOpen())
    {
        // 3. 事件处理（关闭窗口、键盘输入等）
        sf::Event event;
        while (window.pollEvent(event))
        {
            // 处理窗口关闭事件
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            // 处理键盘按键事件（按 ESC 关闭窗口）
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
            }
        }

        // 4. 渲染逻辑
        // 清空窗口（设置背景色为浅灰色）
        window.clear(sf::Color(0,0,0));
        GameStatics::getTextDrawer().printText(window, "测试");
        GameStatics::getTextDrawer().printNum(window, 15,300,300);
        
        // 显示绘制内容（将缓冲区内容渲染到窗口）
        window.display();
    }

    return 0;
}