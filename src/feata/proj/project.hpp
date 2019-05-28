#pragma once

#include "core/comp_def.hpp"
#include "stl/string.hpp"
#include "stl/map.hpp"


namespace core { class ShLibMgr; }
namespace geom { class FeataGeom; }
namespace gui  { class RenderList; }
namespace mesh { class FeataMesh; }
namespace plug { class PluginMgr;
                 class PluginSettings; }
namespace post { class FeataPost; }
namespace solv { class FeataSolv; }

namespace proj
{
    // v1.0: project IO unavailable
    class Project
    {
    public:
        Project(core::ShLibMgr& shlib_mgr);
       ~Project();

        bool LoadProject(const String& file_name);

        geom::FeataGeom* GetGeometry() const;
        mesh::FeataMesh* GetMesh() const;
        solv::FeataSolv* GetSolver() const;
        post::FeataPost* GetPostpr() const;

        plug::PluginSettings*
        GetActivePlugCompSets(const core::ComponentPluggable comp) const;

    private:
        String name_;

        geom::FeataGeom* geom_;
        mesh::FeataMesh* mesh_;
        solv::FeataSolv* solv_;
        post::FeataPost* post_;
    };
}
