// #include "drop.hpp"
// #include <SFML/Graphics.hpp>
// #include <iostream>

// int main() {
//     // 创建SFML窗口
//     sf::RenderWindow window(sf::VideoMode(800, 600), "SFML文件拖放示例");
    
//     // 启用文件拖放功能
//     enableFileDrop(window);
    
//     // 主循环
//     while (window.isOpen()) {
//         // 处理事件
//         sf::Event event;
//         while (window.pollEvent(event)) {
//             if (event.type == sf::Event::Closed) {
//                 window.close();
//             }
//         }
        
//         // 设置文件拖放回调函数
//         handleFileDropMessages([](const std::string& filePath) {
//             std::cout << "file pull in " << filePath << std::endl;
//             // 在这里添加文件处理逻辑
//         });
        
//         // 清空窗口
//         window.clear(sf::Color::White);
        
//         // 绘制内容
//         // ...
        
//         // 显示窗口
//         window.display();
//     }

//     return 0;
// }
#include "base/gJson.h"
int main() {
  Gjson gs("res\\jsonTest.json");
  gs["a"] = 21;
  gs["b"] = 22;
  gs["c"] = nlohmann::json();
  gs["c"].push_back(1);
  gs["c"].push_back(11);
  gs["c"].push_back(23);
  gs.save();
  
  return 0;}