#pragma once


class TopoDS_Shape;

namespace geom::mdl
{
    //! \param LinearDeflection
    //!    linear deflection.
    //! \param IsRelative
    //!    if TRUE deflection used for discretization of
    //!    each edge will be <theLinDeflection> * <size of edge>. Deflection
    //!    used for the faces will be the maximum deflection of their edges.
    //! \param AngularDeflection
    //!    angular deflection.
    //! \param Parallelism
    //!    if TRUE shape will be meshed in parallel.
    struct ConvertToStlInfo
    {
        double LinearDeflection = 0.5;
        double AngularDeflection = 0.5;
        bool IsRelative = false;
        bool Parallelism = true;

        static constexpr double MinLinearDeflection = 1e-5;
    };

    bool ConvertStepToStl(const TopoDS_Shape& step,
                          const ConvertToStlInfo& info,
                          TopoDS_Shape& stl);
}
