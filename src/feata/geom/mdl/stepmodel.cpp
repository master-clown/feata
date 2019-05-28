#include "geom/mdl/stepmodel.hpp"
#include <STEPControl_Reader.hxx>
#include <TopoDS_Shape.hxx>
#include "util/filesys.hpp"
#include "util/log.hpp"


namespace geom::mdl
{
    StepModel::StepModel()
        : reader_(new STEPControl_Reader())
    {}

    StepModel::StepModel(const String& file_name)
        : StepModel()
    {
        Load(file_name);
    }

    StepModel::~StepModel()
    {
        SAFE_DEL(reader_);
    }

    bool StepModel::Load(const String& file_path)
    {
        using util::logger::PrintE;
        using util::logger::PrintI;

        if(!util::filesys::IsFileExists(file_path))
        {
            PrintE("STEP model file '%1' does not exist."_qs.arg(file_path));
            return false;
        }

        const auto name { util::filesys::GetFileName(file_path) };
        const auto tmp_fp { file_path.toStdString() };
        const auto read_st = reader_->ReadFile(tmp_fp.c_str());
        if(read_st != IFSelect_ReturnStatus::IFSelect_RetDone)
        {
            PrintE("STEP model '%1': failed to read the file. Status %2."_qs.arg(name).arg(read_st));
            return false;
        }
        const auto ntrans = reader_->TransferRoots();
        if(!ntrans)
        {
            PrintE("STEP model '%1': failed to transfer roots."_qs.arg(name));
            return false;
        }

        PrintI("STEP model '%1' is loaded. Transfered roots number: %2."_qs.arg(name).arg(ntrans));

        return true;
    }

    TopoDS_Shape* StepModel::ToOCCModel()
    {
        return new TopoDS_Shape(reader_->OneShape());
    }
}
