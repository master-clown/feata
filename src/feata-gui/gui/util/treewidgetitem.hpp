#pragma once

#include <QTreeWidgetItem>
#include "core/comp_def.hpp"
#include "plug/plugid_t.hpp"
#include "stl/map.hpp"
#include "gui/util/treewidgetitem_type.hpp"


namespace gui
{
    class TreeWidgetItem
            : public QTreeWidgetItem
    {
    public:
        TreeWidgetItem(QTreeWidgetItem* parent,
                       const TreeWidgetItemType type,
                       const plug::plugid_t plugin_id = plug::PLUGIN_ID_NULL);
        TreeWidgetItem(QTreeWidget* parent,
                       const TreeWidgetItemType type,
                       const plug::plugid_t plugin_id = plug::PLUGIN_ID_NULL);
        TreeWidgetItem(const TreeWidgetItem& itm) = delete;
        TreeWidgetItem& operator=(const TreeWidgetItem& rhs) = delete;

        void SetDep(const int dep_id, const TreeWidgetItemType type);
        void SetPluginId(const plug::plugid_t plugin_id);
        void SetReadyIcon(const bool is_ready);

        int GetId() const;
        int GetDepId() const;
        TreeWidgetItemType GetType() const;
        TreeWidgetItemType GetDepType() const;
        core::Component GetCompType() const;
        plug::plugid_t GetPluginId() const;

        static core::Component ConvertTypeToComp(const TreeWidgetItemType type);
        static String GetTreeItemTypePrefixName(const TreeWidgetItemType type);

    private:
        int id_ = 0;
        int dep_id_ = 0;        // dependent component id
        TreeWidgetItemType dep_type_;
        plug::plugid_t plugin_id_;

        void Init_();
        void SetId_(const int new_id);

        static Map<TreeWidgetItemType, int> AvailIdMap_;

        using Base = QTreeWidgetItem;
    };
}
