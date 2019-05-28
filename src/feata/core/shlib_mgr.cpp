#include "core/shlib_mgr.hpp"


namespace core
{
    ShLibMgr::~ShLibMgr()
    {
        Clear();
    }

    bool ShLibMgr::Load(const String& name,
                        const String& file_name,
                        const bool local_search)
    {
        auto lib_info = new ShLibInfo();
        lib_info->Lib.Load(file_name, local_search);

        if(lib_info->Lib.IsOpen())
        {
            lib_lst_.insert(name, lib_info);
            return true;
        }

        delete lib_info;
        return false;
    }

    void ShLibMgr::Unload(const String& name)
    {
        if(lib_lst_.contains(name))
        {
            delete lib_lst_[name];
            lib_lst_.remove(name);
        }
    }

    void ShLibMgr::Clear() { for(auto& v: lib_lst_) delete v; lib_lst_.clear(); }

    ShLibInfo* ShLibMgr::operator[](const String& name) { return lib_lst_.contains(name) ? lib_lst_[name] : nullptr; }
}
