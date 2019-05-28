#pragma once

#include "stl/vector.hpp"


struct SolvInfo;

namespace post
{
    struct NodalSol
    {
        SolvInfo* Sol;
        Vector<double> DataSet;
    };
}
