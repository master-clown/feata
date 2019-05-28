#pragma once

#include "geom/point.hpp"
#include "stl/string.hpp"
#include "util/dirty_int.hpp"


namespace geom::mdl { struct ConvertToStlInfo;
                      struct ShapeTriangulation; }
namespace geom      { class GeomVis; }
namespace util      { class ParallelTask; }

namespace geom
{
    class FeataGeom : public util::DirtyInterface   // 'dirty' relates to triangulation
    {
    public:
        FeataGeom();
       ~FeataGeom();

        bool Load(const String& file_name);
        void Clear();

        bool StartPolygonize(util::ParallelTask*& ptask);
        bool ConvertToTriangulation(mdl::ShapeTriangulation* prev_tr,
                                    mdl::ShapeTriangulation*& tr) const;

        void RemovePolygonizedModel();

        mdl::ConvertToStlInfo* GetStlConvertParams() const;
        GeomVis* GetGeomVis() const;
        GeomVis* GetPolygonizedGeomVis() const;
        bool HasModel() const;
        bool HasPolygonizedModel() const;

    private:
       struct FeataGeomPriv;

       FeataGeomPriv* priv_;
       mdl::ConvertToStlInfo* stl_params_;
    };
}
