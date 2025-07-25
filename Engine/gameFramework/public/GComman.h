#ifndef GCOMMAN_H
#define GCOMMAN_H
#include "base/base.h"
inline float getVectorLen(const FVector3 &vec) { return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z); }
inline FVector3 vectorNormalize(const FVector3 &vec) {
    float len = getVectorLen(vec);
    if (len < 0.00001)
        return {0, 0, 0};
    return vec / len;
}
#endif // GCOMMAN_H