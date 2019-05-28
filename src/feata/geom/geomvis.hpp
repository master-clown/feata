#pragma once

#include "geom/gid_t.hpp"
#include "defs/common.hpp"
#include "stl/list.hpp"


class AIS_Shape;
class TopoDS_Shape;

namespace geom
{
    class GeomVis
    {
    public:
        GeomVis();
        GeomVis(TopoDS_Shape* sh);
       ~GeomVis();

        void Init(TopoDS_Shape* sh);
        void Nullify();

        bool IsNull() const;
        bool GetOCCShape(const TopoDS_Shape*& sh) const;
        AIS_Shape* GetOCCModel() const;

        List<gid_t> GetVertexId(const List<const TopoDS_Shape*>& sh_lst) const;
        List<gid_t> GetEdgeId(const List<const TopoDS_Shape*>& sh_lst) const;
        List<gid_t> GetFaceId(const List<const TopoDS_Shape*>& sh_lst) const;
        List<gid_t> GetSolidId(const List<const TopoDS_Shape*>& sh_lst) const;
//        bool GetVertexByID(const gid_t id, AIS_Shape& sh) const;
//        bool GetEdgeByID(const gid_t id, AIS_Shape& sh) const;
//        bool GetFaceByID(const gid_t id, AIS_Shape& sh) const;
//        bool GetSolidByID(const gid_t id, AIS_Shape& sh) const;
        unsigned int GetVertexNum() const;
        unsigned int GetEdgeNum() const;
        unsigned int GetFaceNum() const;
        unsigned int GetSolidNum() const;

    private:
        struct GeomVisPriv;

        GeomVisPriv* priv_ = nullptr;
    };
}
