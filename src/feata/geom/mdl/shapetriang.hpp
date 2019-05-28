#pragma once

#include "defs/common.hpp"
#include "geom/gid_t.hpp"
#include "stl/vector.hpp"


struct MeshInfo;

namespace geom::mdl
{
    struct FaceTriangulation
    {
        gid_t FaceMarker;                 // one-based face index (for BC)
        int Triangle[3];
    };

    struct ShapeTriangulation
    {
        Vector<real> NodeLst;                   // 3 real per 1 node
        Vector<FaceTriangulation> FaceTriaLst;
        real InternalPoint[3];          //a random point, to distinguish regions

        //uint CountTriangles() const;
        bool IsValid() const;
        void ToMeshInfo(MeshInfo& info) const;
    };
}
