#pragma once

#include "geom/point.hpp"


namespace geom
{
    template<class num_t = real>
    struct Rect
    {
        Point2<num_t> LTOrigin;
        num_t Width;
        num_t Height;


        Rect() = default;
        Rect(const Point2<num_t>& lt_orig,
             const num_t width,
             const num_t height)
            : LTOrigin(lt_orig)
            , Width(width)
            , Height(height)
        {}
        Rect(const Point2<num_t>& corner1,
             const Point2<num_t>& corner2)
        {
            const Point2<num_t>* cptrl = &corner1,
                               * cptrr = &corner2,
                               * tmp;

            if(corner1[0] > corner2[0])
            {
                tmp = cptrl;
                cptrl = cptrr;
                cptrr = tmp;
            }

            LTOrigin[0] = (*cptrl)[0];
            LTOrigin[1] = (*cptrl)[1] < (*cptrr)[1] ? (*cptrl)[1] : (*cptrr)[1];
            Width = (*cptrr)[0] - (*cptrl)[0];
            Height = ((*cptrl)[1] < (*cptrr)[1] ? (*cptrr)[1] : (*cptrl)[1]) - LTOrigin[1];
        }

        Point2<num_t> GetCornerLT() const { return LTOrigin; }
        Point2<num_t> GetCornerLB() const { return { LTOrigin[0], LTOrigin[1] + Height}; }
        Point2<num_t> GetCornerRT() const { return { LTOrigin[0] + Width, LTOrigin[1]}; }
        Point2<num_t> GetCornerRB() const { return { LTOrigin[0] + Width, LTOrigin[1] + Height}; }

    };

    using Recti = Rect<int>;
}
