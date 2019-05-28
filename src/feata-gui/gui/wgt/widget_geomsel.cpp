#include "gui/wgt/widget_geomsel.hpp"
#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include "plug/pluginsets_types.hpp"
#include "proj/glob_events.hpp"
#include "util/log.hpp"
#include "util/tablewidget.hpp"


using util::logger::PrintE;

namespace gui::wgt
{
    WidgetGeomSel::WidgetGeomSel(const plug::PluginSettingValue& val,
                                 QWidget* parent)
        : WidgetGeomSel(parent)
    {
        Init(val);
    }

    WidgetGeomSel::~WidgetGeomSel()
    {
        Clear_();
    }

    bool WidgetGeomSel::Init(const plug::PluginSettingValue& val)
    {
        if(val.Type != plug::PluginSettingType::GeomSel)
        {
            PrintE("Cannot init geometry selection widget: wrong type '%1'."_qs
                   .arg(TO_STRING(val.Type)));
            return false;
        }
        if(val.FullName.isEmpty())
        {
            PrintE("Cannot init geometry selection widget: full name is not specified.");
            return false;
        }

        const auto& val_geomsel = std::get<plug::PluginSTGeomSel>(val.Value);
        if((geom_type_ = val_geomsel.EntType) == geom::GEOM_ENT_TYPE_INVALID)
        {
            PrintE("Cannot init geometry selection widget: selection entity type is not"
                   "specified.");
            return false;
        }

        Clear_();

        title_lbl_ = new QLabel(val.FullName);
        title_lbl_->setStyleSheet("font-weight: bold;");
        desc_lbl_ = new QLabel(val.Desc);
        title_lbl_->setWordWrap(true);
        desc_lbl_->setWordWrap(true);

        group_box_ = new QGroupBox("Selection");
        group_box_->setCheckable(true);
        group_box_->setChecked(false);

        table_ = new TableWidget(0, 1);
        table_->setShowGrid(false);
        table_->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
        table_->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectItems);
        table_->horizontalHeader()->hide();
        //table_->verticalHeader()->hide();
        table_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        table_->verticalHeader()->setDefaultSectionSize(24);
        table_->setEditTriggers(QTableWidget::NoEditTriggers);


        auto pb_clear = new QPushButton("Clear");
        auto pb_remove = new QPushButton("Remove");

        auto pb_lout = new QHBoxLayout();
        pb_lout->addWidget(pb_clear);
        pb_lout->addWidget(pb_remove);
        pb_lout->addStretch();

        auto gb_lout = new QVBoxLayout();
        gb_lout->addWidget(table_);
        gb_lout->addLayout(pb_lout);
        group_box_->setLayout(gb_lout);

        auto lout = new QVBoxLayout(this);
        lout->addWidget(title_lbl_);
        lout->addWidget(group_box_);
        lout->addStretch();
        lout->addWidget(desc_lbl_);
        lout->addStretch();

        setLayout(lout);

        connect(group_box_, &QGroupBox::toggled, this,
        [this](const bool is_on)
        {
            Base::SetSelectionState(is_on);
            emit SelectionToggled(is_on);
            if(!is_on) table_->clearSelection();
        });
        connect(table_, &QTableWidget::itemSelectionChanged, this,
        [this]()
        {
            if(delete_flag)
            {
                table_->clearSelection();
                delete_flag = false;
            }

            const geom::gid_t id = table_->selectedItems().isEmpty() ?
                                       geom::GEOM_ID_NULL : table_->selectedItems().first()->text().toInt();
            proj::glev::GeomEntSelectedInLst.Raise(id);
        });
        eid_sel_changed_ = proj::glev::GeomEntSelectionUpdated.AddFollower(
        [this](const List<geom::gid_t>& id_lst)
        {
            if(!GetSelectionState())
                return;

            bool changed = false;

            List<geom::gid_t> tbl_id_lst;
            for(int i_row = 0; i_row < table_->rowCount(); ++i_row)
            {
                const auto item = table_->item(i_row, 0);
                const geom::gid_t id = item->text().toInt();

                if(!id_lst.contains(id))
                {
                    delete_flag = true;
                    table_->removeRow(i_row--);
                    changed = true;
                }
                else
                    tbl_id_lst.append(id);
            }
            for(const auto& id: id_lst)
                if(!tbl_id_lst.contains(id))
                {
                    auto item = new QTableWidgetItem(TO_STRING(id));
                    table_->insertRow(table_->rowCount());
                    table_->setItem(table_->rowCount()-1, 0, item);
                    changed = true;
                }

            if(changed) emit BecameDirty();
        });
        connect(pb_clear, &QPushButton::clicked, this,
        [this]()
        {
            if(QMessageBox::question(this, "Clear selection", "Are you sure you want to clear selection?")
               == QMessageBox::No)
                return;

            emit BecameDirty();

            table_->setRowCount(0);
            proj::glev::GeomEntUnselectAll.Raise();
        });
        connect(pb_remove, &QPushButton::clicked, this,
        [this]()
        {
            if(const auto lst = table_->selectedItems(); !lst.isEmpty())
            {
                emit BecameDirty();

                proj::glev::GeomEntUnselectOne.Raise(lst.first()->text().toInt());
                delete_flag = true;
                table_->removeRow(lst.first()->row());
            }
        });

        // to enable selection
        //emit group_box_->toggled(true);

        return true;
    }

    void WidgetGeomSel::SetSelectionState(const bool is_on)
    {
        Base::SetSelectionState(is_on);

        if(!is_on)
        {
            QSignalBlocker sb(this);
            group_box_->setChecked(false);
            table_->clearSelection();
        }
    }

    bool WidgetGeomSel::ToPlugSettValue(plug::PluginSettingValue& val) const
    {
        if(!IsValid())
            return false;

        plug::PluginSTGeomSel val_gsel(geom_type_);
        for(int i_row = 0; i_row < table_->rowCount(); ++i_row)
        {
            const geom::gid_t id = table_->item(i_row, 0)->text().toInt();
            val_gsel.IdLst.append(id);
        }

        val.Value = val_gsel;

        return true;
    }

    bool WidgetGeomSel::IsValid() const { return title_lbl_ && !title_lbl_->text().isEmpty(); }

    WidgetGeomSel::WidgetGeomSel(QWidget* parent)
        : Base(parent)
    {}

    void WidgetGeomSel::hideEvent(QHideEvent* ev)
    {
        Base::hideEvent(ev);
        group_box_->setChecked(false);
    }

    void WidgetGeomSel::Clear_()
    {
        proj::glev::GeomEntSelectionUpdated.RemoveFollower(eid_sel_changed_);
        eid_sel_changed_ = EVENT_ID_INVALID;

        if(layout())
            Base::DestroyLayout(layout());

        table_ = nullptr;
        group_box_ = nullptr;
        desc_lbl_ = nullptr;
        title_lbl_ = nullptr;
    }
}
