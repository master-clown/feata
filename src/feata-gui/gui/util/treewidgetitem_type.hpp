#pragma once


namespace gui
{
    enum TreeWidgetItemType
    {
        TREE_WIDGET_ITEM_TYPE_GEOM = 1001,      // min because of Qt
        TREE_WIDGET_ITEM_TYPE_TRNG,
        TREE_WIDGET_ITEM_TYPE_MESH,
        TREE_WIDGET_ITEM_TYPE_SOLV,
        TREE_WIDGET_ITEM_TYPE_POST
    };
}
