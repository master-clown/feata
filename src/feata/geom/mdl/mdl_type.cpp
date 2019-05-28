#include "geom/mdl/mdl_type.hpp"
#include "util/filesys.hpp"


namespace geom::mdl
{
    MdlType MdlTypeByFilename(const String& file)
    {
        const auto ext { util::filesys::GetFileExt(file).toLower() };
        if(ext.contains("step") || ext.contains("stp"))
            return MDL_TYPE_STEP;
        if(ext.contains("stl"))
            return MDL_TYPE_STL;

        return MDL_TYPE_NONE;
    }

    bool IsPolygonBased(const MdlType type)
    {
        return type == MDL_TYPE_STL;
    }
}
