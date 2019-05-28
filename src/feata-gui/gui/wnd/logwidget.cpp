#include "gui/wnd/logwidget.hpp"
#include <QPlainTextEdit>


namespace gui::wnd
{
    LogWidget::LogWidget(QWidget* parent)
        : Base(parent)
    {
        connect(this, &LogWidget::Print, this, &LogWidget::PrintMsg,
                Qt::ConnectionType::QueuedConnection);
    }

    LogWidget::~LogWidget()
    {}

    void LogWidget::PrintMsg(String msg, const int type)
    {
        msg = msg.toHtmlEscaped();
        msg.replace("\t", "    ")
           .replace("\n", "<br>");
        const String prfx = type == 0 ? "" : "<span style=\"background-color: "_qs +
                                             (type == 1 ? "#FFFF00\"><font color=\"#000000\">" :
                                              type == 2 ? "#FF0000\"><font color=\"#FFFFFF\">" : "");
        appendHtml("<pre>" + prfx + msg + (type > 0 ? "</font></span>" : "") + "</pre>");
    }
}
