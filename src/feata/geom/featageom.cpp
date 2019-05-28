#include "geom/featageom.hpp"
#include <TopoDS_Shape.hxx>
#include "geom/geomvis.hpp"
#include "geom/mdl/convert.hpp"
#include "geom/mdl/mdl_type.hpp"
#include "geom/mdl/shapetriang.hpp"
#include "geom/mdl/stepmodel.hpp"
#include "geom/mdl/stlmodel.hpp"
#include "geom/mdl/triangulate.hpp"
#include "proj/glob_events.hpp"
#include "util/log.hpp"
#include "util/paralleltask.hpp"


namespace geom
{
    struct FeataGeom::FeataGeomPriv
    {
        GeomVis* Shape;
        GeomVis* StlShape;
        mdl::MdlType  ShapeMdlType = mdl::MDL_TYPE_NONE;
        TopoDS_Shape* ShapeOccPtr = nullptr;
        TopoDS_Shape* StlShapeOccPtr = nullptr;

        FeataGeomPriv()
            : Shape(new GeomVis())
            , StlShape(new GeomVis())
        {}
       ~FeataGeomPriv()
        {
            SAFE_DEL(StlShape);
            SAFE_DEL(Shape);
            SAFE_DEL(StlShapeOccPtr);
            SAFE_DEL(ShapeOccPtr);
        }
    };

    FeataGeom::FeataGeom()
    {
        priv_ = new FeataGeomPriv();
        stl_params_ = new mdl::ConvertToStlInfo();
        stl_params_->LinearDeflection = -1.0;   // invalidate
    }

    FeataGeom::~FeataGeom()
    {
        SAFE_DEL(stl_params_);
        SAFE_DEL(priv_);
    }

    bool FeataGeom::Load(const String& file_name)
    {
        Clear();

        if((priv_->ShapeMdlType = mdl::MdlTypeByFilename(file_name))
           == mdl::MDL_TYPE_NONE)
        {
            util::logger::PrintE("Error during load of '%1' model: unknown file type."_qs
                                 .arg(file_name));
            return false;
        }

        std::unique_ptr<mdl::BaseModel> m;
        if(priv_->ShapeMdlType == mdl::MDL_TYPE_STEP)
            m.reset(new mdl::StepModel());
        else
        if(priv_->ShapeMdlType == mdl::MDL_TYPE_STL)
            m.reset(new mdl::StlModel());

        if(!m->Load(file_name))
            return false;

        priv_->Shape->Init(priv_->ShapeOccPtr = m->ToOCCModel());

        MakeDirty();

        return true;
    }

    void FeataGeom::Clear()
    {
        MakeDirty();

        RemovePolygonizedModel();
        priv_->Shape->Nullify();
        SAFE_DEL(priv_->ShapeOccPtr);
        priv_->ShapeMdlType = mdl::MDL_TYPE_NONE;
    }

    bool FeataGeom::StartPolygonize(util::ParallelTask*& ptask)
    {
        using util::logger::PrintE;

        if(priv_->ShapeMdlType == mdl::MDL_TYPE_NONE)
        {
            PrintE("Error during shape polygonalization: "
                   "no geometry is loaded.");
            return false;
        }
        else
        if(priv_->ShapeMdlType == mdl::MDL_TYPE_STL)
        {
            PrintE("Shape polygonalization is cancelled: "
                   "the geometry does not require it.");
            return true;        // everything is OK, already have poly
        }

        if(stl_params_->LinearDeflection < geom::mdl::ConvertToStlInfo::MinLinearDeflection)
        {
            PrintE("Error during shape polygonalization: "
                   "minimum value for linear deflection is %1."_qs
                   .arg(geom::mdl::ConvertToStlInfo::MinLinearDeflection));
            return false;
        }

        if(!ptask)
            ptask = new util::ParallelTask();
        ptask->SetFunction(
        [this](const QAtomicInt&)
        {
            RemovePolygonizedModel();

            priv_->StlShapeOccPtr = new TopoDS_Shape();
            if(!mdl::ConvertStepToStl(*priv_->ShapeOccPtr, *stl_params_, *priv_->StlShapeOccPtr))
            {
                SAFE_DEL(priv_->StlShapeOccPtr);
                return;
            }
            priv_->StlShape->Init(priv_->StlShapeOccPtr);
        });
        ptask->SetOnFinishedFunc(
        [this]
        {
            MakeUndirty();
            proj::glev::TriangulationFinished.Raise();
        });

        ptask->start();
        return true;
    }

    bool FeataGeom::ConvertToTriangulation(mdl::ShapeTriangulation* prev_tr,
                                           mdl::ShapeTriangulation*& tr) const
    {
        SAFE_DEL(prev_tr);
        tr = nullptr;

        const TopoDS_Shape* sh;
        if(!GetPolygonizedGeomVis()->GetOCCShape(sh) || !sh)
            return false;

        tr = new mdl::ShapeTriangulation();
        mdl::Triangulate(*sh, *tr);

        return true;
    }

    void FeataGeom::RemovePolygonizedModel()
    {
        // remove if the main shape is not poly itself
        if(priv_->ShapeMdlType == mdl::MDL_TYPE_STL)
            return;

        MakeDirty();

        priv_->StlShape->Nullify();
        SAFE_DEL(priv_->StlShapeOccPtr);
    }

    mdl::ConvertToStlInfo* FeataGeom::GetStlConvertParams() const { return stl_params_; }
    GeomVis* FeataGeom::GetGeomVis() const { return priv_->Shape; }
    GeomVis* FeataGeom::GetPolygonizedGeomVis() const
    {
        return priv_->ShapeMdlType == mdl::MDL_TYPE_STL ? priv_->Shape : priv_->StlShape;
    }

    bool FeataGeom::HasModel() const { return priv_->Shape->IsNull(); }
    bool FeataGeom::HasPolygonizedModel() const
    { return (priv_->ShapeMdlType == mdl::MDL_TYPE_STL ? priv_->Shape : priv_->StlShape)->IsNull(); }
}
