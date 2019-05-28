#pragma once

#include "stl/string.hpp"


class TopoDS_Shape;

namespace geom::mdl
{
    class BaseModel
    {
    public:
        virtual ~BaseModel() = default;

        virtual bool Load(const String& file_path) = 0;

        /**
         * @return
         *      Accept the result as OCC handle, or take care
         *      of deleting it after usage.
         */
        virtual TopoDS_Shape* ToOCCModel() = 0;
    };
}
