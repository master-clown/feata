#pragma once

#include "defs/common.hpp"
#include "stl/string.hpp"


namespace gui::dlg
{
    enum class MboxBtn      // from Qt
    {
        NoButton           = 0x00000000,
        Ok                 = 0x00000400,
        Save               = 0x00000800,
        SaveAll            = 0x00001000,
        Open               = 0x00002000,
        Yes                = 0x00004000,
        YesToAll           = 0x00008000,
        No                 = 0x00010000,
        NoToAll            = 0x00020000,
        Abort              = 0x00040000,
        Retry              = 0x00080000,
        Ignore             = 0x00100000,
        Close              = 0x00200000,
        Cancel             = 0x00400000,
        Discard            = 0x00800000,
        Help               = 0x01000000,
        Apply              = 0x02000000,
        Reset              = 0x04000000,
        RestoreDefaults    = 0x08000000,
    };
    MAKE_ENUM_FLAGGED(MboxBtn, int);

    MboxBtn MboxQuestion(const String& title,
                         const String& msg,
                         MboxBtn btns = MboxBtn::Yes | MboxBtn::No,
                         MboxBtn focus_btn = MboxBtn::NoButton);
    MboxBtn MboxCritical(const String& title,
                         const String& msg,
                         MboxBtn btns = MboxBtn::Ok,
                         MboxBtn focus_btn = MboxBtn::NoButton);
    MboxBtn MboxInform(const String& title,
                       const String& msg,
                       MboxBtn btns = MboxBtn::Ok,
                       MboxBtn focus_btn = MboxBtn::NoButton);
    MboxBtn MboxWarning(const String& title,
                        const String& msg,
                        MboxBtn btns = MboxBtn::Ok,
                        MboxBtn focus_btn = MboxBtn::NoButton);
}
