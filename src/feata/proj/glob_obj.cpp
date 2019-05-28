#include "proj/glob_obj.hpp"


namespace proj::globj
{
    Project* gProject = nullptr;
    core::ComponentPluggable gCurrentComponent =
            core::COMPONENT_PLUGGABLE_ENUM_COUNT;
}
