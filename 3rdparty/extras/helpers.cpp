
#include "helpers.h"
#include <algorithm>

namespace helpers {
float map(float value,
                 float min1, float max1,
                 float min2, float max2)
{
    if (max1 == min1)
        return min2;   // degenerate input range

    float t = (value - min1) / (max1 - min1);
    float out = min2 + t * (max2 - min2);

    // Clamp to output bounds
    if (min2 < max2)
        return std::clamp(out, min2, max2);
    else
        return std::clamp(out, max2, min2); // handle reversed ranges
}
}
