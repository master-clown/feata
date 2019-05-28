#pragma once

#include "geom/mdl/basemodel.hpp"


namespace geom::mdl
{
    class StlModel
            : public BaseModel
    {
    public:
        StlModel();
        StlModel(const String& file_name);
       ~StlModel() override;

        bool Load(const String& file_path) override;
        TopoDS_Shape* ToOCCModel() override;    // caller deletes ptr

    protected:
        struct StlModelPriv;

        StlModelPriv* priv_;
    };
}
