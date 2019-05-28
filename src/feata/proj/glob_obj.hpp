#pragma once

#include "core/comp_def.hpp"

namespace proj { class Project; }

namespace proj::globj
{
    extern Project* gProject;
    extern core::ComponentPluggable gCurrentComponent;      // which is working right at the moment
}
