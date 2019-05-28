#include "cmdarglist.hpp"
#include "defs/common.hpp"


namespace util
{
    CmdArgList::CmdArgList(const uint argc,
                           char* argv[])
    {
        if(!argc)
            return;

        resize(argc);

        for(uint i = 0; i < argc; ++i)
            (*this)[i] = argv[i];
    }
}
