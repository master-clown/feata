#include "geom/mdl/convert.hpp"
#include <TopoDS_Shape.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include "util/log.hpp"


using util::logger::PrintE;

namespace geom::mdl
{
    bool ConvertStepToStl(const TopoDS_Shape& step,
                          const ConvertToStlInfo& info,
                          TopoDS_Shape& stl)
    {
        BRepMesh_IncrementalMesh msh(step, info.LinearDeflection,
                                     info.IsRelative,
                                     info.AngularDeflection,
                                     info.Parallelism);

        if(const auto stat = msh.GetStatusFlags();
           stat != BRepMesh_Status::BRepMesh_NoError)
        {
            PrintE("Error while converting STEP to STL. Status code: %1"_qs.arg(stat));
            return false;
        }

        stl = msh.Shape();

        return true;
    }
}
