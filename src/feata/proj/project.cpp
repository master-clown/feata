#include "proj/project.hpp"
#include "data/json.hpp"
#include "geom/featageom.hpp"
#include "mesh/featamesh.hpp"
#include "mesh/mesher.hpp"
#include "post/featapost.hpp"
#include "solv/featasolv.hpp"
#include "solv/solver.hpp"
#include "util/filesys.hpp"
#include "util/log.hpp"


using util::logger::PrintE;

namespace proj
{
    Project::Project(core::ShLibMgr& shlib_mgr)
        : geom_(new geom::FeataGeom())
        , mesh_(new mesh::FeataMesh(shlib_mgr))
        , solv_(new solv::FeataSolv(shlib_mgr))
        , post_(new post::FeataPost())
    {}

    Project::~Project()
    {
        SAFE_DEL(post_);
        SAFE_DEL(solv_);
        SAFE_DEL(mesh_);
        SAFE_DEL(geom_);
    }

    bool Project::LoadProject(const String& file_name)
    {
        const auto obj = data::json::ParseFromString(util::filesys::ReadFile(file_name)).object();
        if(obj.isEmpty())
        {
            PrintE("Cannot load project file '%1'"_qs.arg(file_name));
            return false;
        }

        name_ = obj["name"].toString();

        if(!geom_->Load(obj["geom_file"].toString()))
            return false;

        return true;
    }

    geom::FeataGeom* Project::GetGeometry() const { return geom_; }
    mesh::FeataMesh* Project::GetMesh() const     { return mesh_; }
    solv::FeataSolv* Project::GetSolver() const   { return solv_; }
    post::FeataPost* Project::GetPostpr() const   { return post_; }

    plug::PluginSettings*
    Project::GetActivePlugCompSets(const core::ComponentPluggable comp) const
    {
        const auto mesher = mesh_->GetCurrentMesher();
        const auto solver = solv_->GetCurrentSolver();

        switch(comp)
        {
        case core::COMPONENT_PLUGGABLE_MESH:
            return mesher ? mesher->GetPluginSettings() : nullptr;
        case core::COMPONENT_PLUGGABLE_SOLV:
            return solver ? solver->GetPluginSettings() : nullptr;
        default:
            return nullptr;
        }
    }
}
