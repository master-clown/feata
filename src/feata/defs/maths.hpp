#pragma once

#include "defs/common.hpp"

#define PI 3.141592653589793238

#define DEG_TO_RAD(deg) deg * PI / 180.0
#define RAD_TO_DEG(rad) rad * 180.0 / PI


template<class NumT>
inline NumT Clamp(const NumT& a,
					 const NumT& mini,
					 const NumT& maxi)
{
    return a <= mini ? mini :
                       a >= maxi ? maxi : a;
}
