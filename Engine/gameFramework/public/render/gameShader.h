#ifndef SPRITEUSESHADER_H
#define SPRITEUSESHADER_H
#include "gameStatics.h"
#include "gsource.h"
#include "nlohmann_json/json.hpp"
#include "sprite.h"
#include <fstream>
class ShaderFrag : public GSourceObj {
  sf::Shader shader;

public:
  ShaderFrag() {}
  void init(const std::string &path_) {
    shader.loadFromFile(path_, sf::Shader::Fragment);
  }
  sf::Shader *getShader() { return &shader; }
};
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
class GResourceShader : public GReourceTree<ShaderFrag> {
public:
  static GResourceShader &getResourceShaders() {
    static GResourceShader ret;
    return ret;
  }
  inline static sf::Shader defaultShader;
  GResourceShader() {
    auto strss = collectFiles("res/shaders", ".frag");
    for (std::vector<std::string> &strs : strss) {
      const std::string path = strs[0];
      emplace(path).init(path);
    }
  }
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
inline sf::Texture tesTest;
class GameShaderInst {
public:
  sf::Shader *shader = nullptr;
  std::map<std::string, float> properties;
  std::map<std::string, std::string> textures;
  GameShaderInst() {}
  GameShaderInst(const std::string &shaderInstJson_) { init(shaderInstJson_); }
  virtual ~GameShaderInst() {}
  virtual void init(const std::string &shaderInstJson_) {
    shader = &(GResourceShader::defaultShader);
    nlohmann::json jsObj;
    std::ifstream ifile;
    ifile.open(shaderInstJson_);
    ifile >> jsObj;
    ifile.close();
    ///////////////////////////////////////////////////////////读取
    const std::string &fragPath = jsObj["fragPath"];
    for (auto &property : jsObj["properties"].items()) {
      properties[property.key()] = property.value().get<float>();
    }
    for (auto &texture : jsObj["textures"].items()) {
      textures[texture.key()] = texture.value().get<std::string>();
    }
    ////////////////////////////////////////////////////////////设置
    shader =
        (GResourceShader::getResourceShaders().getObject(fragPath).getShader());
    for (auto &p : properties) {
      shader->setUniform(p.first, p.second);
    }
    for (auto &t : textures) {
      sf::Texture &tex = GSource::getSource().getObject(t.second).texture;
      tex.setRepeated(true);
      shader->setUniform(t.first, tex);
    }
  }
  void draw(GSprite &spr_, GameWindow &window_) {
    shader->setUniform("time", GameStatics::getGameClcok().getElapsedTime().asSeconds());
    window_.draw(spr_.getSpriteBase(), shader);
    sf::RectangleShape shape;
    window_.draw(shape, shader);
  }
  void setValueScalarByname(const std::string &name_, float value_) {
    shader->setUniform(name_, value_);
  }
  void setValueVectorBynaem(const std::string &name_, const FVector4 &vec4_) {
    shader->setUniform(name_, vec4_);
  }
};
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#endif // SPRITEUSESHADER_H