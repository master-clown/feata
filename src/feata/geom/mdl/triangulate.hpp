#pragma once

#include "defs/common.hpp"


class TopoDS_Shape;

namespace geom::mdl
{
    struct ShapeTriangulation;

    /**
     * Requires 'sh' to be already polygonized, \sa 'ConvertFrom<...>ToStl()'
     */
    void Triangulate(const TopoDS_Shape& sh,
                     ShapeTriangulation& tria);
}
