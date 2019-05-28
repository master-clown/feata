#pragma once


namespace gui
{
    enum SelectCmd
    {
        SELECT_CMD_NONE,
        SELECT_CMD_SINGLE,
        SELECT_CMD_RECT_ADD,        // when adds objects in rect to selection
        SELECT_CMD_RECT_REM         // when removes
    };

    enum SelectEntityType
    {
        SELECT_ENTITY_TYPE_VERTEX = 1,
        SELECT_ENTITY_TYPE_EDGE   = 2,
        SELECT_ENTITY_TYPE_FACE   = 4,
        SELECT_ENTITY_TYPE_SOLID  = 6,
        SELECT_ENTITY_TYPE_NONE   = -1
    };
}
