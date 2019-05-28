#pragma once

#include "defs/common.hpp"
#include "stl/list.hpp"


template<uint dim, class NumT = coord_t>
class Point
{
public:
    Point(const List<NumT>& coord_lst)
        : Point()
    {
        const auto size = std::min(dim, coord_lst.size());
        for(uint i = 0; i < size; ++i)
            coord_[i] = coord_lst[i];
    }

    Point(const NumT& x,
          const NumT& y = {},
          const NumT& z = {},
          const NumT& w = {})
        : Point()
    {
        coord_[0] = x;
        if(dim > 1) coord_[1] = y;
        if(dim > 2) coord_[2] = z;
        if(dim > 3) coord_[3] = w;
    }

    NumT& operator[](const int i) { return coord_[i >= 0 ? i : (dim + i)]; }
    const NumT& operator[](const int i) const { return coord_[i >= 0 ? i : (dim + i)]; }

private:
    Point()
    {
        static_assert (dim >= 0, "Zero dimensionality forbidden");
    }

    NumT coord_[dim];
};

using Point2D = Point<2>;
using Point3D = Point<3>;
using Point4D = Point<4>;

template<uint dim, class NumT = coord_t>
using Vec  = Point<dim, NumT>;
using Vec2 = Point<2>;
using Vec3 = Point<3>;
using Vec4 = Point<4>;
