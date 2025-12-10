#include "GComman.h"
#include <cstdarg>


FVector3 vectorNormalize(const FVector3 &vec) {
    float len = getVectorLen(vec);
    if (len < 0.00001)
        return {0, 0, 0};
    return vec / len;
}
float getVectorLen(const FVector3 &vec) { return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z); }
