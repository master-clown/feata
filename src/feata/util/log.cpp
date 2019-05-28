#include <iostream>
#include <fstream>
#include <mutex>
#include "util/log.hpp"
#include "util/datetime.hpp"
#include "gui/wnd/logwidget.hpp"


namespace util::logger
{
    static LogType gLogType;
    static String gLogFileName;
    static gui::wnd::LogWidget* gLogWidget;
    static bool gLogIsVerbose = false;
    static std::mutex gLogLock;

    static void PrintLog(const String& msg,
                         const int type = 0);		// 0: msg, 1: warn, 2: error, 3: misc

    bool Init(const LogInitInfo& info)
    {
        gLogIsVerbose = info.IsVerbose;
        gLogType = info.Type;
        gLogFileName = info.LogFile;
        gLogWidget = info.LogWidget;

        if((gLogType == LOG_TYPE_FILE && gLogFileName.isEmpty())
           || (gLogType == LOG_TYPE_WIDGET && !gLogWidget))
            return false;

        return true;
    }

    void Free()
    {
        return;
    }

    void SetVerboseLog(bool is_verbose)
    {
        gLogIsVerbose = is_verbose;
    }

    void Print(const String& msg)
    {
        PrintLog(msg);
    }

    void PrintW(const String& msg)
    {
        PrintLog(msg, 1);
    }

    void PrintE(const String& msg)
    {
        PrintLog(msg, 2);
    }

    void PrintI(const String& msg)
    {
        PrintLog(msg, 3);
    }

    void PrintLog(const String& msg,
                  const int type)
    {
        if(gLogType == LOG_TYPE_NONE)
            return;
        if(type == 3 && !gLogIsVerbose)
            return;

        String str{ util::time::GetCurrentDateTime("dd/MM/yy hh:mm:ss:zzz") + "> " };

        if(type == 1)
            str += "[WARNING] ";
        else
        if(type == 2)
            str += "[ERROR] ";

        str += msg;

        if(gLogType == LOG_TYPE_WIDGET) // already thread-safe
        {
            emit gLogWidget->Print(str, type);
            return;
        }

        str.replace("\n", "\n\t");
        //str += "\n";

        std::lock_guard<std::mutex> lg(gLogLock);

        if(gLogType == LOG_TYPE_CONSOLE)
            std::wcout << str.toStdWString();
        else
        if(gLogType == LOG_TYPE_FILE)
        {
            std::wofstream ofs(gLogFileName.toStdString(), std::ios::app);
            assert(ofs.is_open());

            ofs << str.toStdWString();
            ofs.close();
        }
    }
}
