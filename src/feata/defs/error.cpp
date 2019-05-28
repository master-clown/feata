#include "defs/error.hpp"
#include <windows.h>


int FeataApiLastError()
{
    return GetLastError();
}
