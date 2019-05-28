#include "geom/geomvis.hpp"
#include <AIS_Shape.hxx>
#include <Prs3d_LineAspect.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include "defs/common.hpp"
#include "geom/geom_ent_type.hpp"
#include "stl/map.hpp"


namespace geom
{
    template<TopAbs_ShapeEnum occ_type>
    static List<gid_t> GetGeomObjId(const TopoDS_Shape* sh,
                                    const List<const TopoDS_Shape*>& sh_lst,
                                    const uint num_obj);

    struct GeomVis::GeomVisPriv
    {
        // if ever rendered, OCC will delete it
        AIS_Shape* Shape;

        uint VertexNum;
        uint EdgeNum;
        uint FaceNum;
        uint SolidNum;

        ~GeomVisPriv() { Nullify(); }
        void Nullify() { Shape = nullptr; VertexNum = EdgeNum = FaceNum = SolidNum = 0; }
    };

    GeomVis::GeomVis() : priv_(new GeomVisPriv())  {}
    GeomVis::GeomVis(TopoDS_Shape* sh) : GeomVis() { Init(sh); }
    GeomVis::~GeomVis()                            { SAFE_DEL(priv_); }

    void GeomVis::Init(TopoDS_Shape* sh)
    {
        if(!priv_) priv_ = new GeomVisPriv();
        priv_->Nullify();

        priv_->Shape = new AIS_Shape(*sh);
        priv_->Shape->SetColor(Quantity_NOC_BROWN);

        for(TopExp_Explorer expl(*sh, TopAbs_VERTEX); expl.More(); expl.Next(), ++priv_->VertexNum);
        for(TopExp_Explorer expl(*sh, TopAbs_EDGE); expl.More(); expl.Next(), ++priv_->EdgeNum);
        for(TopExp_Explorer expl(*sh, TopAbs_FACE); expl.More(); expl.Next(), ++priv_->FaceNum);
        for(TopExp_Explorer expl(*sh, TopAbs_SOLID); expl.More(); expl.Next(), ++priv_->SolidNum);
    }

    void GeomVis::Nullify() { if(priv_) priv_->Nullify(); }

    bool GeomVis::IsNull() const { return priv_ && priv_->Shape; }

    bool GeomVis::GetOCCShape(const TopoDS_Shape*& sh) const
    { if(!priv_) return false; sh = &priv_->Shape->Shape(); return true; }

    AIS_Shape* GeomVis::GetOCCModel() const { return priv_ ? priv_->Shape : nullptr; }

    List<gid_t> GeomVis::GetVertexId(const List<const TopoDS_Shape*>& sh_lst) const
    {
        const TopoDS_Shape* shape;
        GetOCCShape(shape);

        return GetGeomObjId<TopAbs_VERTEX>(shape, sh_lst, priv_->VertexNum);
    }

    List<gid_t> GeomVis::GetEdgeId(const List<const TopoDS_Shape*>& sh_lst) const
    {
        const TopoDS_Shape* shape;
        GetOCCShape(shape);

        return GetGeomObjId<TopAbs_EDGE>(shape, sh_lst, priv_->EdgeNum);
    }

    List<gid_t> GeomVis::GetFaceId(const List<const TopoDS_Shape*>& sh_lst) const
    {
        const TopoDS_Shape* shape;
        GetOCCShape(shape);

        return GetGeomObjId<TopAbs_FACE>(shape, sh_lst, priv_->FaceNum);
    }

    List<gid_t> GeomVis::GetSolidId(const List<const TopoDS_Shape*>& sh_lst) const
    {
        const TopoDS_Shape* shape;
        GetOCCShape(shape);

        return GetGeomObjId<TopAbs_SOLID>(shape, sh_lst, priv_->SolidNum);
    }

    unsigned int GeomVis::GetVertexNum() const  { return priv_ ? priv_->VertexNum : 0; }
    unsigned int GeomVis::GetEdgeNum() const    { return priv_ ? priv_->EdgeNum : 0; }
    unsigned int GeomVis::GetFaceNum() const    { return priv_ ? priv_->FaceNum : 0; }
    unsigned int GeomVis::GetSolidNum() const   { return priv_ ? priv_->SolidNum : 0; }


    template<TopAbs_ShapeEnum occ_type>
    List<gid_t> GetGeomObjId(const TopoDS_Shape* sh,
                             const List<const TopoDS_Shape*>& sh_lst,
                             const uint num_obj)
    {
        if(!num_obj || sh_lst.isEmpty())
            return {};

        uint num_found = 0;
        const uint num = sh_lst.size();
        List<gid_t> id_lst;
        for(uint i = 0; i < num; ++i) id_lst << GEOM_ID_NULL;

        uint id = 1;
        for(TopExp_Explorer expl(*sh, occ_type); expl.More() && num_found < num; expl.Next(), ++id)
            for(uint i = 0; i < num; ++i)
                if(id_lst[i] == GEOM_ID_NULL && expl.Current() == *sh_lst[i])
                {
                    id_lst[i] = id;
                    ++num_found;
                    break;
                }

        return id_lst;
    }
}
