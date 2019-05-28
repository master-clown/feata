#pragma once

#include "stl/string.hpp"


namespace util
{
    using shlib_t = void*;

    class SharedLib
    {
    public:
        SharedLib() = default;
        SharedLib(const String& file_name,
                  const bool local_search);
        ~SharedLib();

        SharedLib(SharedLib&& lib);
        SharedLib& operator=(SharedLib&& lib);
        SharedLib(const SharedLib&) = delete;
        SharedLib& operator=(const SharedLib&) = delete;

        bool Load(const String& file_name,
                  const bool local_search);
        void Free();
        void* FindSymbol(const String& name);

        bool IsOpen() const;
        shlib_t GetHandle() const;

    private:
        shlib_t handle_;
    };
}
