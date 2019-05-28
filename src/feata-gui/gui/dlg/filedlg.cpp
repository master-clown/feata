#include "gui/dlg/filedlg.hpp"
#include <QFileDialog>

namespace gui::dlg
{
    bool OpenFileDlg(const String& caption,
                     const String& filter,
                     String& out)
    {
        if((out = QFileDialog{}.getOpenFileName(nullptr, caption, {}, filter)) == "")
            return false;

        return true;
    }

    bool SaveFileDlg(const String& caption,
                     const String& filter,
                     String& out)
    {
        if((out = QFileDialog{}.getSaveFileName(nullptr, caption, {}, filter)) == "")
            return false;

        return true;
    }

    bool OpenFolderDlg(const String& caption,
                       String& out)
    {
        if((out = QFileDialog{}.getExistingDirectory(nullptr, caption)) == "")
            return false;

        return true;
    }
}
