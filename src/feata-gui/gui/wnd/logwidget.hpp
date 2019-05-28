#pragma once

#include <QPlainTextEdit>
#include "stl/string.hpp"


namespace gui::wnd
{
    class LogWidget
            : public QPlainTextEdit
    {
    public:
        LogWidget(QWidget* parent = nullptr);
        ~LogWidget() override;

    signals:
        void Print(const String& msg, const int type = 0);

    private slots:
        void PrintMsg(String msg, const int type = 0);     // 0: msg, 1: warn, 2: error

    private:
        using Base = QPlainTextEdit;
        Q_OBJECT
    };
}
