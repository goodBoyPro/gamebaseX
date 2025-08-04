#ifndef FILE_DROP_HANDLER_H
#define FILE_DROP_HANDLER_H

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

// 回调函数类型定义
using FileDropCallback = std::function<void(const std::string&)>;

// 为窗口启用文件拖放功能（Windows平台）
void enableFileDrop(sf::Window& window);

// 处理窗口消息，检测文件拖放事件（Windows平台）
void handleFileDropMessages(FileDropCallback callback);

#endif // FILE_DROP_HANDLER_H
    