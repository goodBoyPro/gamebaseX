#ifndef GSOURCE_H
#define GSOURCE_H
#include "gstring.h"
#include "sprite.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <thread>
#include <vector>

class GSourceIF {
public:
  std::vector<std::string> splitString(const std::string &filename);
  std::vector<std::vector<std::string>>
  collectFiles(const std::string &directory, const std::string &extension);
  virtual ~GSourceIF() {}
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <class T> class GReourceTree : public GSourceIF {
protected:
  std::map<size_t, T> data;
  T defaultObj;

public:
  std::atomic<bool> isloadComplete = false;
  T &getObject(size_t id) {
    auto it = data.find(id);
    if (it == data.end())
      return defaultObj;

    return it->second;
  }
  T &getObject(const Gstring &str) {
    auto it = data.find(str.get_hash());
    if (it == data.end())
      return defaultObj;
    return it->second;
  }
  T &emplace(const std::string &path_) {
    size_t id = Gstring::calculateHash(path_);
    const auto &pair =
        data.emplace(std::piecewise_construct, std::forward_as_tuple(id),
                     std::forward_as_tuple());
    if (!pair.second) {
      throw std::overflow_error("Resource error: hash confilcted--" + path_);
    }
    pair.first->second.idAndPath = path_;
    return pair.first->second;
  }
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
class GSource : public GReourceTree<GTexture> {
  enum Info {
    epath,
    etype, //
    eid,
    elevel,
    erow,
    ecolumn,
    ecenterX,
    ecenterY,
    ename,

    count
  };

public:
  static GSource &getSource() {
    static GSource sr;
    return sr;
  }
  void loadResource() {
    std::vector<std::vector<std::string>> vec = collectFiles("res", ".png");
    for (std::vector<std::string> &cvec : vec) {
      const std::string &path = cvec[0];
      GTexture &gtex = emplace(path);
      if (cvec.size() != count) {
        gtex.init(1, 1, 0, 0, path);
        continue;
      }
      try {
        int row = std::stoi(cvec[erow]);
        int column = std::stoi(cvec[ecolumn]);
        float centerX = std::stof(cvec[ecenterX]);
        float centerY = std::stof(cvec[ecenterY]);

        gtex.init(row, column, centerX, centerY, path);

      } catch (const std::exception &e) {
        std::cerr << "invalid file! " << e.what() << std::endl;
        gtex.init(1, 1, 0, 0, path);
        continue;
      }
    }
    isloadComplete = true;
  }
  std::thread *th;
  GSource() {
    th = new std::thread(&GSource::loadResource, this);
    
    th->detach();
  }
  ~GSource() { delete th; }
};
#endif // GSOURCE_H