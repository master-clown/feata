#pragma once

#include "stl/string.hpp"


namespace gui::wnd
{
    class LogWidget;
}

namespace util::logger
{
    enum LogType
    {
        LOG_TYPE_NONE,
        LOG_TYPE_CONSOLE,
        LOG_TYPE_FILE,
        LOG_TYPE_WIDGET
    };

    struct LogInitInfo
    {
        bool IsVerbose;
        LogType Type;

        String LogFile;
        gui::wnd::LogWidget* LogWidget;
    };

    bool Init(const LogInitInfo& info);
    void Free();
    void SetVerboseLog(bool is_verbose);

    void Print(const String& msg);		// Message
    void PrintW(const String& msg);		// Warning
    void PrintE(const String& msg);		// Error
    void PrintI(const String& msg);		// Misc information
}

