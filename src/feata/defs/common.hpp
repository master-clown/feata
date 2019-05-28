#pragma once

#include <cstdint>

#define UNUSED_PARAM(p) (void)p;
#define SAFE_DEL(ptr) if(ptr) { delete ptr; ptr = nullptr; }
#define SAFE_DEL_ARR(ptr) if(ptr) { delete[] ptr; ptr = nullptr; }

#define FLT_EPS FLT_EPSILON
#define DBL_EPS DBL_EPSILON

#define FLT_EQL(x, y) fabs(x - y) < FLT_EPS
#define DBL_EQL(x, y) fabs(x - y) < DBL_EPS

#define MAKE_ENUM_FLAGGED(EnumT, UnderNumT)        \
    inline EnumT operator~ (EnumT a) { return (EnumT)~(UnderNumT)a; }                                   \
    inline EnumT operator| (EnumT a, EnumT b) { return (EnumT)((UnderNumT)a | (UnderNumT)b); }          \
    inline EnumT operator& (EnumT a, EnumT b) { return (EnumT)((UnderNumT)a & (UnderNumT)b); }          \
    inline EnumT operator^ (EnumT a, EnumT b) { return (EnumT)((UnderNumT)a ^ (UnderNumT)b); }          \
    inline EnumT& operator|= (EnumT& a, EnumT b) { return (EnumT&)((UnderNumT&)a |= (UnderNumT)b); }    \
    inline EnumT& operator&= (EnumT& a, EnumT b) { return (EnumT&)((UnderNumT&)a &= (UnderNumT)b); }    \
    inline EnumT& operator^= (EnumT& a, EnumT b) { return (EnumT&)((UnderNumT&)a ^= (UnderNumT)b); }    \


using int8 = int8_t;
using uint8 = uint8_t;
using int16 = int16_t;
using uint16 = uint16_t;
using int32 = int32_t;
using uint32 = uint32_t;
using uint = uint32;
using int64 = int64_t;
using uint64 = uint64_t;
using sbyte = int8;
using ubyte = uint8;

using real32 = float;
using real64 = double;
using real = double;

using coord_t = real;
