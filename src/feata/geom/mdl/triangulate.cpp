#include "geom/mdl/triangulate.hpp"
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <Poly_Triangulation.hxx>
#include <Precision.hxx>
#include "geom/mdl/shapetriang.hpp"

#define INT_POINT_TOLERANCE_FACTOR 100


namespace geom::mdl
{
    static void FindIntPoint(const TopoDS_Shape& sh,
                             const gp_Pnt& bp,
                             const gp_Dir& normal,
                             ShapeTriangulation& tria);

    void Triangulate(const TopoDS_Shape& sh,
                     ShapeTriangulation& tria)
    {
        int nb_nodes = 0;
        int nb_trias = 0;                       // triangles

        // count nodes and triangles
        for(TopExp_Explorer expl(sh, TopAbs_FACE); expl.More(); expl.Next())
        {
            TopLoc_Location loc;
            const auto& trian = BRep_Tool::Triangulation(TopoDS::Face(expl.Current()), loc);
            if (!trian.IsNull())
            {
                nb_nodes += trian->NbNodes();
                nb_trias += trian->NbTriangles();
            }
        }

        tria.NodeLst.clear();            tria.FaceTriaLst.clear();
        tria.NodeLst.resize(3*nb_nodes); tria.FaceTriaLst.resize(nb_trias);
        int off_node = 0;
        int i_tria = 0;
        int face_id = 1;
        bool found_int_point = false;

        for(TopExp_Explorer expl(sh, TopAbs_FACE); expl.More(); expl.Next(), ++face_id)
        {
            TopLoc_Location loc;
            const auto& trian = BRep_Tool::Triangulation (TopoDS::Face(expl.Current()), loc);
            const auto& nodes = trian->Nodes();
            const auto& trias = trian->Triangles();

            // copy nodes
            const auto& trsf = loc.Transformation();
            for (int i_n = nodes.Lower(), i_nfirst = i_n; i_n <= nodes.Upper(); ++i_n)
            {
                auto pnt = nodes(i_n);
                pnt.Transform(trsf);

                tria.NodeLst[3*(i_n - i_nfirst + off_node) + 0] = pnt.X();
                tria.NodeLst[3*(i_n - i_nfirst + off_node) + 1] = pnt.Y();
                tria.NodeLst[3*(i_n - i_nfirst + off_node) + 2] = pnt.Z();
            }

            const auto orient = expl.Current().Orientation();
            for (int i_t = trias.Lower(); i_t <= trias.Upper(); ++i_t, ++i_tria)
            {
                const auto& tri = trias(i_t);

                int n_ids[3];
                tri.Get(n_ids[0], n_ids[1], n_ids[2]);

                if(orient == TopAbs_REVERSED)   // ccw or cw? (1st required)
                    std::swap(n_ids[1], n_ids[2]);

                tria.FaceTriaLst[i_tria].Triangle[0] = (n_ids[0] + off_node);
                tria.FaceTriaLst[i_tria].Triangle[1] = (n_ids[1] + off_node);
                tria.FaceTriaLst[i_tria].Triangle[2] = (n_ids[2] + off_node);
                tria.FaceTriaLst[i_tria].FaceMarker = face_id;
            }

            off_node += nodes.Size();

            if(!found_int_point)
            {
                // trying to get triangle center
                const auto& nl= tria.NodeLst;
                const auto& t = tria.FaceTriaLst[0].Triangle;
                const gp_Pnt pnt((nl[3*t[0]+0] + nl[3*t[1]+0] + nl[3*t[2]+0]) / 3.0,
                                 (nl[3*t[0]+1] + nl[3*t[1]+1] + nl[3*t[2]+1]) / 3.0,
                                 (nl[3*t[0]+2] + nl[3*t[1]+2] + nl[3*t[2]+2]) / 3.0);

                FindIntPoint(sh, pnt, trian->Normal(1), tria);
                found_int_point = true;
            }
        }
    }

    void FindIntPoint(const TopoDS_Shape& sh,
                      const gp_Pnt& bp,
                      const gp_Dir& normal,
                      ShapeTriangulation& tria)
    {
        const real f = INT_POINT_TOLERANCE_FACTOR*Precision::Confusion();
        const gp_Pnt pnt_1 {bp.Translated(+f*normal)};
        const gp_Pnt pnt_2 {bp.Translated(-f*normal)};
        const gp_Pnt* res = &pnt_2;

        if(BRepClass3d_SolidClassifier(sh, pnt_1, Precision::Confusion()).State()
           == TopAbs_IN)
        {
            res = &pnt_1;
        }

        tria.InternalPoint[0] = res->X();
        tria.InternalPoint[1] = res->Y();
        tria.InternalPoint[2] = res->Z();
    }
}
