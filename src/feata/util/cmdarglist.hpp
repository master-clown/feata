#pragma once

#include "stl/vector.hpp"
#include "stl/string.hpp"


namespace util
{
    class CmdArgList
            : public Vector<String>
    {
    public:
        CmdArgList() = default;
        CmdArgList(const uint argc,
                   char* argv[]);

    private:
        using Base = StringList;
    };
}
