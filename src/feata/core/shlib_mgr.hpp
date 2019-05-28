#pragma once

#include "util/shared_lib.hpp"
#include "stl/hashtable.hpp"
#include "stl/string.hpp"


namespace core
{
    struct ShLibInfo
    {
        String FileName;
        util::SharedLib Lib;
    };

    class ShLibMgr
    {
    public:
        ShLibMgr() = default;
        ~ShLibMgr();

        bool Load(const String& name,
                  const String& file_name,
                  const bool local_search);
        void Unload(const String& name);
        void Clear();

        ShLibInfo* operator[](const String& name);

    private:
        HashTable<String, ShLibInfo*> lib_lst_;
    };
}
