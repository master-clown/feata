#include "util/shared_lib.hpp"
#include "util/filesys.hpp"
#include "defs/arch.hpp"

#if defined(FEATA_SYSTEM_WIN)
    #include <Windows.h>
#elif defined(FEATA_SYSTEM_UNIX)
    #include <dlfcn.h>
#endif


namespace util
{
    SharedLib::SharedLib(const String& file_name,
                         const bool local_search)
    {
        Load(file_name, local_search);
    }

    SharedLib::~SharedLib()
    {
        Free();
    }

    SharedLib::SharedLib(SharedLib&& lib)
    {
        handle_ = lib.handle_;
        lib.handle_ = nullptr;
    }

    SharedLib& SharedLib::operator=(SharedLib&& lib)
    {
        handle_ = lib.handle_;
        lib.handle_ = nullptr;

        return *this;
    }

    bool SharedLib::Load(const String& file_name, const bool local_search)
    {
        if(local_search && !util::filesys::IsFileExists(file_name))
            return false;

        if(IsOpen())
            Free();

#if defined(FEATA_SYSTEM_WIN)
        const auto ws { file_name.toStdWString() };
        handle_ = LoadLibraryW(ws.c_str());
#elif defined(FEATA_SYSTEM_UNIX)
        const auto str { file_name.toStdString() };
        handle_ = dlopen(str.c_str(), RTLD_LAZY);
#endif

        return IsOpen();
    }

    void SharedLib::Free()
    {
        if(!IsOpen())
            return;

#if defined(FEATA_SYSTEM_WIN)
        FreeLibrary(static_cast<HMODULE>(handle_));
#elif defined(FEATA_SYSTEM_UNIX)
        dlclose(handle_);
#endif

        handle_ = nullptr;
    }

    void* SharedLib::FindSymbol(const String& name)
    {
        if(!IsOpen())
            return nullptr;

        const auto str { name.toStdString() };

#if defined(FEATA_SYSTEM_WIN)
        return (void*)GetProcAddress(static_cast<HMODULE>(handle_), str.c_str());
#elif defined(FEATA_SYSTEM_UNIX)
        return dlsym(handle_, str.c_str());
#endif
    }

    bool SharedLib::IsOpen() const       { return handle_ != nullptr; }
    shlib_t SharedLib::GetHandle() const { return handle_; }
}
