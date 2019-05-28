#pragma once

#include "stl/string.hpp"


namespace gui::dlg
{
    bool OpenFileDlg(const String& caption,
                     const String& filter,
                     String& out);
    bool SaveFileDlg(const String& caption,
                     const String& filter,
                     String& out);
    bool OpenFolderDlg(const String& caption,
                       String& out);
}
