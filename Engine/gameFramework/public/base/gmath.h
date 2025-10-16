#ifndef GMATH_H
#define GMATH_H
#include "string"
namespace Gmath {
inline float strToFloat() {}
inline std::string numberToStr(float value_) { return std::to_string(value_); }
inline std::string numberToStr(int value_) { return std::to_string(value_); }
}; // namespace Gmath
#endif //  GMATH_H