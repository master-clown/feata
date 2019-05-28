#include "plug/services.hpp"
#include "proj/glob_events.hpp"
#include "proj/glob_obj.hpp"
#include "proj/project.hpp"
#include "plug/pluginmgr.hpp"
#include "plug/pluginsets.hpp"
#include "util/log.hpp"


namespace plug::service
{
    static int gProgressBarMin = -1;
    static int gProgressBarMax = -1;

    int Log(const char* msg, const unsigned int size, char**, unsigned int*)
    {
        util::logger::Print(String::fromLatin1(msg, size));
        return 0;
    }

    int LogI(const char* msg, const unsigned int size, char**, unsigned int*)
    {
        util::logger::PrintI(String::fromLatin1(msg, size));
        return 0;
    }

    int LogW(const char* msg, const unsigned int size, char**, unsigned int*)
    {
        util::logger::PrintW(String::fromLatin1(msg, size));
        return 0;
    }

    int LogE(const char* msg, const unsigned int size, char**, unsigned int*)
    {
        util::logger::PrintE(String::fromLatin1(msg, size));
        return 0;
    }

    int InitProgressBar(const char* msg, const unsigned int size, char**, unsigned int*)
    {
        int init_val = gProgressBarMax = gProgressBarMin = -1;
        const auto lst = String::fromLatin1(msg, size).split(",");
        for(const auto& arg: lst)
        {
            const auto kv = arg.split(":");
            if(kv.size() != 2)
                continue;

            int* cur_arg = nullptr;
            if(kv.first() == "min") cur_arg = &gProgressBarMin;
            else
                if(kv.first() == "max") cur_arg = &gProgressBarMax;
                else
                    if(kv.first() == "cur") cur_arg = &init_val;
                    else goto lbl_error;

            bool to_int;
            *cur_arg = kv[1].toInt(&to_int);
            if(!to_int || *cur_arg < 0)
                goto lbl_error;
        }

        proj::glev::ProgressBarInitRequested.Raise(gProgressBarMin, gProgressBarMax);
        if(init_val >= 0)
            proj::glev::ProgressBarChangeValueRequested.Raise(init_val);

        return 0;

lbl_error:
        gProgressBarMax = gProgressBarMin = -1;
        return 1;
    }

    int SetProgressValue(const char*, const unsigned int progress, char**, unsigned int*)
    {
        if((int)progress < gProgressBarMin || gProgressBarMax < (int)progress)
            return 1;

        proj::glev::ProgressBarChangeValueRequested.Raise(progress);
        return 0;
    }

    int HideProgressBar(const char*, const unsigned int, char**, unsigned int*)
    {
        gProgressBarMin = gProgressBarMax = -1;
        proj::glev::ProgressBarHideRequested.Raise();
        return 0;
    }

    int GetSettingValue(const char* sett_name,
                        const unsigned int name_len,
                        char** val,
                        unsigned int* val_size)
    {
        *val = nullptr; *val_size = 0;

        if(!proj::globj::gProject || proj::globj::gCurrentComponent ==
           core::COMPONENT_PLUGGABLE_ENUM_COUNT)
            return 1;
        const auto psets = proj::globj::gProject->GetActivePlugCompSets(proj::globj::gCurrentComponent);
        if(!psets || !psets->IsValid())
            return 2;

        const auto name {String::fromLatin1(sett_name, name_len)};

        String val_str;
        if(!psets->GetToString(name, val_str))
            return 3;

        *val = new char[1 + (*val_size = val_str.size())];
        for(uint i = 0; i < *val_size; ++i)
            (*val)[i] = val_str[i].toLatin1();
        (*val)[*val_size] = '\0';

        psets->GetPluginMgr().AddAllocPtr(*val);

        return 0;
    }

    int FreeMemory(const char* ptr, const unsigned int, char**, unsigned int*)
    {
        if(!proj::globj::gProject || proj::globj::gCurrentComponent ==
           core::COMPONENT_PLUGGABLE_ENUM_COUNT)
            return 1;
        const auto psets = proj::globj::gProject->GetActivePlugCompSets(proj::globj::gCurrentComponent);
        if(!psets || !psets->IsValid())
            return 2;

        if(psets->GetPluginMgr().RemoveAllocPtr(ptr))
            delete ptr;

        return 0;
    }
}
