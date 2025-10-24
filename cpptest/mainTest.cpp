#include <SFML/Graphics.hpp>

int main()
{
    // 创建一个800x600像素的窗口，标题为"SFML窗口示例"
    sf::RenderWindow window(sf::VideoMode(800, 600), L"SFML窗口示例");

    // 主循环：只要窗口打开就持续运行
    while (window.isOpen())
    {
        // 事件处理
        sf::Event event;
        while (window.pollEvent(event))
        {
            // 关闭窗口事件
            if (event.type == sf::Event::Closed)
                window.close();
            if(event.type == sf::Event::GainedFocus){
                printf("gained\n");
            }
            if(event.type == sf::Event::LostFocus){
                printf("lost\n");
                // 鼠标按钮释放时的处理
            }
        }

        // 清屏（设置背景为黑色）
        window.clear(sf::Color::Black);

        // 在这里可以添加绘制代码（例如绘制图形、文本等）

        // 显示窗口内容
        window.display();
    }

    return 0;
}