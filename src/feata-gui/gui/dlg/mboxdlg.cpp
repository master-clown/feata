#include "gui/dlg/mboxdlg.hpp"
#include <QMessageBox>


namespace gui::dlg
{
    MboxBtn MboxQuestion(const String& title,
                         const String& msg,
                         MboxBtn btns,
                         MboxBtn focus_btn)
    {
        return (MboxBtn)
        QMessageBox::question(nullptr, title, msg,
                              QMessageBox::StandardButtons((int)btns),
                              QMessageBox::StandardButton((int)focus_btn));
    }

    MboxBtn MboxCritical(const String& title,
                         const String& msg,
                         MboxBtn btns,
                         MboxBtn focus_btn)
    {
        return (MboxBtn)
        QMessageBox::critical(nullptr, title, msg,
                              QMessageBox::StandardButtons((int)btns),
                              QMessageBox::StandardButton((int)focus_btn));
    }

    MboxBtn MboxInform(const String& title,
                       const String& msg,
                       MboxBtn btns,
                       MboxBtn focus_btn)
    {
        return (MboxBtn)
        QMessageBox::information(nullptr, title, msg,
                                 QMessageBox::StandardButtons((int)btns),
                                 QMessageBox::StandardButton((int)focus_btn));
    }

    MboxBtn MboxWarning(const String& title,
                        const String& msg,
                        MboxBtn btns,
                        MboxBtn focus_btn)
    {
        return (MboxBtn)
        QMessageBox::warning(nullptr, title, msg,
                             QMessageBox::StandardButtons((int)btns),
                             QMessageBox::StandardButton((int)focus_btn));
    }
}
