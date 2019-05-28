#pragma once

#include "geom/mdl/basemodel.hpp"


class STEPControl_Reader;

namespace geom::mdl
{
    class StepModel
            : public BaseModel
    {
    public:
        StepModel();
        StepModel(const String& file_name);
       ~StepModel() override;

        bool Load(const String& file_path) override;
        TopoDS_Shape* ToOCCModel() override;    // caller deletes ptr

    protected:
        STEPControl_Reader* reader_;
    };
}
