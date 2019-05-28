#pragma once


namespace plug::service
{
    int Log(const char* msg, const unsigned int size, char** = nullptr, unsigned int* = nullptr);
    int LogI(const char* msg, const unsigned int size, char** = nullptr, unsigned int* = nullptr);
    int LogW(const char* msg, const unsigned int size, char** = nullptr, unsigned int* = nullptr);
    int LogE(const char* msg, const unsigned int size, char** = nullptr, unsigned int* = nullptr);

    int InitProgressBar(const char* msg, const unsigned int size, char** = nullptr, unsigned int* = nullptr);
    int SetProgressValue(const char*, const unsigned int progress, char** = nullptr, unsigned int* = nullptr);
    int HideProgressBar(const char* = nullptr, const unsigned int = 0, char** = nullptr, unsigned int* = nullptr);

    int GetSettingValue(const char* sett_name,
                        const unsigned int name_len,
                        char** val,
                        unsigned int* val_size);
    int FreeMemory(const char* ptr, const unsigned int = 0, char** = nullptr, unsigned int* = nullptr);
}



