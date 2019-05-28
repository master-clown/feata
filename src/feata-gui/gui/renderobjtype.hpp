#pragma once

#include "defs/common.hpp"


namespace gui
{
    enum RenderObjType
    {
        RENDER_OBJ_TYPE_NONE = 0,
        RENDER_OBJ_TYPE_SHAPE = 1 << 0,
        RENDER_OBJ_TYPE_TRIAN = 1 << 1,
        RENDER_OBJ_TYPE_MESH = 1 << 2,
        RENDER_OBJ_TYPE_SOLU = 1 << 3
    };
    MAKE_ENUM_FLAGGED(RenderObjType, int)
}
