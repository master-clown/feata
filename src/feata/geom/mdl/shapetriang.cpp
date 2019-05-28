#include "geom/mdl/shapetriang.hpp"
#include "mesh/meshinfo.hpp"


namespace geom::mdl
{
    bool ShapeTriangulation::IsValid() const
    { return !NodeLst.isEmpty() && !FaceTriaLst.isEmpty(); }

    void ShapeTriangulation::ToMeshInfo(MeshInfo& info) const
    {
        if(!IsValid())
            return;

        info.Dealloc();
        info.Nullify();

        info.NodeLst = new real[3*(info.NodeNum = NodeLst.size() / 3)];
        for(uint i = 0; i < 3*info.NodeNum; ++i)
            info.NodeLst[i] = NodeLst[i];

#pragma message("If to modify the program for multiple solids and materials, here it is")
        info.RegionLst = new real[5*(info.RegionNum = 1)] {
            InternalPoint[0],
            InternalPoint[1],
            InternalPoint[2],
            1, 0.0
        };

        info.FacetLst = new MeshInfo::Facet[info.FacetNum = FaceTriaLst.size()];
        info.FacetMarkerLst = new MeshInfo::mrk_t[info.FacetNum];
        for(uint i = 0; i < info.FacetNum; ++i)
        {
            info.FacetMarkerLst[i] = FaceTriaLst[i].FaceMarker;

            auto& ft = info.FacetLst[i];
            ft.PolyLst = new MeshInfo::Polygon[ft.PolyNum = 1];
            ft.PolyLst[0].VertexLst = new MeshInfo::id_t[ft.PolyLst[0].VertexNum = 3];
            ft.PolyLst[0].VertexLst[0] = FaceTriaLst[i].Triangle[0];
            ft.PolyLst[0].VertexLst[1] = FaceTriaLst[i].Triangle[1];
            ft.PolyLst[0].VertexLst[2] = FaceTriaLst[i].Triangle[2];
        }
    }
}
