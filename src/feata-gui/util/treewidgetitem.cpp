#include "util/treewidgetitem.hpp"


namespace gui
{
    Map<TreeWidgetItemType, int> TreeWidgetItem::AvailIdMap_;

    TreeWidgetItem::TreeWidgetItem(QTreeWidgetItem* parent,
                                   const TreeWidgetItemType type,
                                   const plug::plugid_t plugin_id)
        : Base(parent, type)
        , plugin_id_(plugin_id)
    {
        Init_();
    }
    TreeWidgetItem::TreeWidgetItem(QTreeWidget* parent,
                                   const TreeWidgetItemType type,
                                   const plug::plugid_t plugin_id)
        : Base(parent, type)
        , plugin_id_(plugin_id)
    {
        Init_();
    }

    void TreeWidgetItem::SetDep(const int dep_id, const TreeWidgetItemType type)
    { dep_id_ = dep_id; dep_type_ = type; }

    void TreeWidgetItem::SetPluginId(const plug::plugid_t plugin_id) { plugin_id_ = plugin_id; SetReadyIcon(false); }

    void TreeWidgetItem::SetReadyIcon(const bool is_ready)
    {
        setIcon(0, QIcon(is_ready ? ":/img/shared/img/accepted.png"
                                  : ":/img/shared/img/rejected.png"));
    }

    plug::plugid_t TreeWidgetItem::GetPluginId() const { return plugin_id_; }

    int TreeWidgetItem::GetId() const { return id_; }
    int TreeWidgetItem::GetDepId() const { return dep_id_; }

    TreeWidgetItemType TreeWidgetItem::GetType() const
    { return static_cast<TreeWidgetItemType>(type()); }

    TreeWidgetItemType TreeWidgetItem::GetDepType() const { return dep_type_; }

    core::Component TreeWidgetItem::GetCompType() const
    {
        return ConvertTypeToComp(GetType());
    }

    core::Component TreeWidgetItem::ConvertTypeToComp(const TreeWidgetItemType type)
    {
        switch (type)
        {
            case TREE_WIDGET_ITEM_TYPE_GEOM: return core::COMPONENT_GEOM;
            case TREE_WIDGET_ITEM_TYPE_TRNG: return core::COMPONENT_TRNG;
            case TREE_WIDGET_ITEM_TYPE_MESH: return core::COMPONENT_MESH;
            case TREE_WIDGET_ITEM_TYPE_SOLV: return core::COMPONENT_SOLV;
            case TREE_WIDGET_ITEM_TYPE_POST: return core::COMPONENT_POST;
        }
    }

    String TreeWidgetItem::GetTreeItemTypePrefixName(const TreeWidgetItemType type)
    {
        return core::GetComponentPrefixName(ConvertTypeToComp(type));
    }

    void TreeWidgetItem::Init_()
    {
        SetId_(++AvailIdMap_[GetType()]);
        setIcon(0, QIcon(":/img/shared/img/rejected.png"));
    }

    void TreeWidgetItem::SetId_(const int new_id) { id_ = new_id; }
}
