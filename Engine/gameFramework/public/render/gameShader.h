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
class GMaterial {

public:
Gstring idAndPath;
  ShaderFrag *shader = nullptr;
  std::map<std::string, float> properties;
  std::map<std::string, std::vector<float>> vec4Properties;
  std::map<std::string, std::string> textures;
  GMaterial() {}
  GMaterial(const std::string &matJson_) { init(matJson_); }
  virtual ~GMaterial() {}
  virtual void init(const std::string &matInstJson_) {
    // shader =
    // &(GResourceShader::defaultShader);///////////////////////////////////////////////需修复
    idAndPath=matInstJson_;
    nlohmann::json jsObj;
    std::ifstream ifile;
    ifile.open(matInstJson_);
    ifile >> jsObj;
    ifile.close();
    ///////////////////////////////////////////////////////////读取
    const std::string &fragPath = jsObj["fragPath"];
    for (auto &property : jsObj["properties"].items()) {
      properties[property.key()] = property.value().get<float>();
    }
    for (auto &property : jsObj["vec4Properties"].items()) {
      vec4Properties[property.key()] =
          property.value().get<std::vector<float>>();
    }
    for (auto &texture : jsObj["textures"].items()) {
      textures[texture.key()] = texture.value().get<std::string>();
    }
    ////////////////////////////////////////////////////////////设置
    shader =
        &(GResourceShader::getResourceShaders().getObject(fragPath));
    for (auto &p : properties) {
      shader->getShader()->setUniform(p.first, p.second);
    }
    for (auto &p : vec4Properties) {
      if (p.second.size() != 4) {
        printf("shader:%s is error type\n", p.first.c_str());
      }
      setValueVectorByname(
          p.first, {p.second[0], p.second[1], p.second[2], p.second[3]});
    }
    for (auto &t : textures) {
      sf::Texture &tex = GSource::getSource().getObject(t.second).texture;
      tex.setRepeated(true);
      shader->getShader()->setUniform(t.first, tex);
    }
  }
  void draw(GSprite &spr_, GameWindow &window_) {
    if(!shader){return;}
    shader->getShader()->setUniform(
        "time", GameStatics::getGameClcok().getElapsedTime().asSeconds());
    window_.draw(spr_.getSpriteBase(), shader->getShader());
    sf::RectangleShape shape;
    window_.draw(shape, shader->getShader());
  }
  void setValueScalarByname(const std::string &name_, float value_) {
    if(!shader)return;
    shader->getShader()->setUniform(name_, value_);
  }
  void setValueVectorByname(const std::string &name_, const FVector4 &vec4_) {
    if(!shader)return;
    shader->getShader()->setUniform(name_, vec4_);
  }
};
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#endif // SPRITEUSESHADER_H