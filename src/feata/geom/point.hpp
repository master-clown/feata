#pragma once

#include "defs/common.hpp"


namespace geom
{
    template<int dim, class num_t = real>
    struct Point
    {
        num_t Coord[dim];

        num_t& operator[](const int idx)             { return idx >= 0 ? Coord[idx] : Coord[dim - idx]; }
        const num_t& operator[](const int idx) const { return idx >= 0 ? Coord[idx] : Coord[dim - idx]; }
    };

    template<class num_t>
    using Point2 = Point<2, num_t>;
    using Point2i= Point2<int>;

    template<class num_t>
    using Point3 = Point<3, num_t>;
    using Point3i= Point3<int>;
}
