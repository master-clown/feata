#include "geom/mdl/stlmodel.hpp"
#include <StlAPI_Reader.hxx>
#include <TopoDS_Shape.hxx>
#include "util/filesys.hpp"
#include "util/log.hpp"


namespace geom::mdl
{
    struct StlModel::StlModelPriv
    {
        TopoDS_Shape Model;
    };

    StlModel::StlModel()
        : priv_(new StlModelPriv())
    {}

    StlModel::StlModel(const String& file_name)
        : StlModel()
    {
        Load(file_name);
    }

    StlModel::~StlModel()
    {
        SAFE_DEL(priv_);
    }

    bool StlModel::Load(const String& file_path)
    {
        using util::logger::PrintE;
        using util::logger::PrintI;

        if(!util::filesys::IsFileExists(file_path))
        {
            PrintE("STL model file '%1' does not exist."_qs.arg(file_path));
            return false;
        }

        const auto name { util::filesys::GetFileName(file_path) };
        const auto tmp_fp { file_path.toStdString() };
        StlAPI_Reader reader;
        if(!reader.Read(priv_->Model, tmp_fp.c_str()))
        {
            PrintE("STL model '%1': failed to read the file."_qs.arg(name));
            return false;
        }

        PrintI("STL model '%1' is loaded."_qs.arg(name));

        return true;
    }

    TopoDS_Shape* StlModel::ToOCCModel()
    {
        return new TopoDS_Shape(priv_->Model);
    }
}
