#pragma once

#include <QWidget>
#include "stl/string.hpp"
#include "stl/map.hpp"


namespace geom::mdl { struct ConvertToStlInfo; }
namespace gui::wgt  { class BaseWidget; }

namespace gui::wgt
{
    /**
     * \details The WidgetTriangulation class
     *     It's used to represent geometry triangulation options
     */
    class WidgetTriangulation
            : public QWidget
    {
    public:
        WidgetTriangulation(QWidget* parent = nullptr);
        WidgetTriangulation(const geom::mdl::ConvertToStlInfo& conv_info,
                            QWidget* parent = nullptr);
        ~WidgetTriangulation() override;

        void Init(const geom::mdl::ConvertToStlInfo& conv_info);

        void ToConvertStlInfo(geom::mdl::ConvertToStlInfo& conv_info);

    public slots:
        void OnDirty();

    private:
        BaseWidget* wgt_lst_[4];    // \sa 'InitGui_()' for correspondence info

        void InitGui_(const geom::mdl::ConvertToStlInfo& conv_info);
        void ClearGui_();

        using Base = QWidget;
    };
}
