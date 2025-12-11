#ifndef BASE_H
#define BASE_H
#include<stdio.h>
#include<stdarg.h>
#include <SFML/Graphics.hpp>
typedef sf::Vector3f FVector3;
typedef sf::Glsl::Vec4 FVector4 ;
typedef sf::Vector2f FVector2;
typedef sf::Vector2i IVector2;
typedef sf::RenderWindow WindowBase;
typedef sf::Event EventBase;
typedef sf::Sprite SpriteBase;
typedef sf::Texture TextureBase;
typedef sf::Color ColorBase;
typedef sf::Clock GameClock;


// 检查文件是否存在
bool isFileExist(const std::string &filePath);
// 检查文件夹是否存在
bool isFolderExist(const std::string &folderPath);
// 如果文件夹不存在则创建
void createFolderIfNotExist(const std::string &folderPath);
// 检查字符串是否合法（不为空且不全是空白字符）
bool isStringValid(const std::string &name_);
// 检查name是否以exp_结尾，如果不是则添加exp_
std::string setExpand(const std::string &name, const std::string &exp_);
// 格式化字符串
std::string formatStr(const char *format, ...);
#endif // BASE_H