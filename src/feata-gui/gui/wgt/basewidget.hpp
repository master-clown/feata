#pragma once

#include <QFrame>


namespace plug { struct PluginSettingValue; }

namespace gui::wgt
{
    class BaseWidget
            : public QFrame
    {
    public:
        BaseWidget(QWidget* parent = nullptr);

        virtual bool Init(const plug::PluginSettingValue& val) = 0;

        /**
         * Assumed the method will have to update only 'val.Value' field
         */
        virtual bool ToPlugSettValue(plug::PluginSettingValue& val) const = 0;
        virtual bool IsValid() const = 0;

        static void DestroyLayout(QLayout* lout);

    signals:
        void BecameDirty();

    private:
        using Base = QFrame;

        Q_OBJECT
    };
}
