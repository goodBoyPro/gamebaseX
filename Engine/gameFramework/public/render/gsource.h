#ifndef GSOURCE_H
#define GSOURCE_H
#include "gstring.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <regex>
#include <thread>
#include <vector>
#include "base/base.h"
#include"base/sourceRefer.h"
class GSourceObj:public SourceIF {
public:
  Gstring idAndPath;

  virtual ~GSourceObj() {}
};

class GSourceTreeIF {
public:
  std::vector<std::string> splitString(const std::string &filename);
  std::vector<std::vector<std::string>>
  collectFiles(const std::string &directory, const std::string &extension);
  virtual ~GSourceTreeIF() {}
  bool isNumber(const std::string &str);
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
template <class T> class GResourceTree : public GSourceTreeIF {
protected:
  std::map<size_t, SourceRefer<T>> data;
  SourceRefer<T> defaultObj;

public:
  ~GResourceTree() {
    for (const std::pair<const size_t, SourceRefer<T>>&ref : data) {
      ref.second.releaseSrc();
    }
  }
  SourceRefer<T> getObject(size_t id) {
    auto it = data.find(id);
    if (it == data.end())
      return defaultObj;

    return it->second;
  }
  SourceRefer<T> getObject(const Gstring &str) {
    auto it = data.find(str.get_hash());
    if (it == data.end()) {
       printf("%s\n",str.getStringStd().c_str());
      SourceRefer<T> s = loadFromPath(str);
     
      return s;
    }
    return it->second;
  }
  void removeObject(const Gstring &str) {
    auto it = data.find(str.get_hash());
    if (it == data.end()) {
      return;
    }
    data.erase(it);
  }
  virtual SourceRefer<T> loadFromPath(const Gstring &path_) {
    printf("error");
    return defaultObj;
  }
 SourceRefer<T> emplace(const std::string &path_,T*newPtr) {
   size_t id = Gstring::calculateHash(path_);
   
    const auto &pair =
        data.emplace(std::piecewise_construct, std::forward_as_tuple(id),
                     std::forward_as_tuple());
    if (!pair.second) {
      throw std::overflow_error("Resource error: hash confilcted--" + path_);
    }
    
    pair.first->second=newPtr->template makeRefer<T>();
    pair.first->second->idAndPath = path_;
    return pair.first->second;
  }
};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // GSOURCE_H