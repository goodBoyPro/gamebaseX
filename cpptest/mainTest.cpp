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
#include"SFML/Graphics.hpp"
int main(){
  sf::RenderWindow window(sf::VideoMode(500,500),"test");
  sf::Event event;


  sf::Shader shader;
  shader.loadFromFile("shader.frag", sf::Shader::Fragment);

  sf::Texture tex;
  tex.loadFromFile("res/test.png");
  tex.setRepeated(true);
  sf::Texture tex2;
  tex2.loadFromFile("res/grass.png");
  tex2.setRepeated(true);
  sf::Sprite spr(tex);






sf::Clock clock;
  while(window.isOpen()){
    while(window.pollEvent(event)){
      if(event.type==sf::Event::Closed)
      window.close();
    }
    shader.setUniform("time", clock.getElapsedTime().asSeconds());
    shader.setUniform("waveStrength", 0.02f);
    shader.setUniform("scale",5.f);
    shader.setUniform("texture", tex);
    shader.setUniform("texture2",tex2);
    shader.setUniform("alpha",0.2f);
    window.clear();
    window.draw(spr,&shader);
    window.display();
  }

  return 0;
}