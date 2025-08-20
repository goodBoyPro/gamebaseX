#ifndef GSTRING_H
#define GSTRING_H
#include <functional>
#include <string>

class Gstring : public std::string {
public:
  template <class T> Gstring(T arg) : std::string(arg) {
    hash = std::hash<std::string>{}(*this);
  }
  Gstring() = default;
  size_t hash;
public:
  static size_t calcutehash(const std::string &str_) {
    return Gstring(str_).hash;
  }

private:
};
#endif // GSTRING_H