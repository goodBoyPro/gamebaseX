#ifndef SPRITEUSESHADER_H
#define SPRITEUSESHADER_H
#include "gameStatics.h"
#include "gsource.h"
#include "nlohmann_json/json.hpp"
#include "sprite.h"
#include <fstream>

class GameShader {
public:
  sf::Shader shader;
  std::map<std::string, float> properties;
  std::map<std::string, int> textures;
  GameShader() { init("res/shaderInst.json"); }
  virtual ~GameShader() {}
  virtual void init(const std::string &shaderInstJson_) {
    nlohmann::json jsObj;
    std::ifstream ifile;
    ifile.open(shaderInstJson_);
    ifile >> jsObj;
    ifile.close();
    const std::string &fragPath = jsObj["fragPath"];
    for (auto &property : jsObj["properties"].items()) {
      properties[property.key()] = property.value().get<float>();
    }
    for (auto &texture : jsObj["textures"].items()) {
      textures[texture.key()] = texture.value().get<float>();
    }
    shader.loadFromFile(fragPath, sf::Shader::Fragment);
    for (auto &p : properties) {
      shader.setUniform(p.first, p.second);
    }
    for (auto &t : textures) {
      sf::Texture &tex = GSource::getSource().getTexture(t.second).texture;
      tex.setRepeated(true);
      shader.setUniform(t.first, tex);
    }
  }
  void draw(GSprite &spr_, GameWindow &window_) {
    shader.setUniform("time",
                      GameStatics::getGameClcok().getElapsedTime().asSeconds());
    window_.draw(spr_.getSpriteBase(), &shader);
  }
};
// class GLandScape {
//   float beginX;
//   float beginY;
//   float widhtTotal;
//   float heightTotal;

// public:
//   GSprite spr;
//   GameShader *gameShader = nullptr;
//   template <class T> void createGameShader(const std::string &fragPath_) {
//     gameShader = new T;
//     gameShader->beginPlaySetArgs(fragPath_);
//   }
//   virtual ~GLandScape() { delete gameShader; }
//   void draw(GameWindow &window_) {
//     float pixSize = window_.getCameraActve()->getPixSize();
//     const IVector2 &winpos = window_.wsToWin({beginX, beginY, 0});
//     spr.setPositionWin(winpos.x, winpos.y);
//     spr.setSizeWin(widhtTotal / pixSize, heightTotal / pixSize);
//     gameShader->tickSetArgs();
//     window_.draw(spr.getSpriteBase(), &(gameShader->shader));
//   }
// };
#endif // SPRITEUSESHADER_H