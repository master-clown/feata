#pragma once

#include "post/nodalsol.hpp"
#include "stl/map.hpp"


namespace post { class NsolMeshVis; }

namespace post
{
    class FeataPost
    {
    public:
        FeataPost() = default;
       ~FeataPost();

        bool RemAllSol(const int solver_id);

        bool AddNsol(const int solver_id, const int dataset_id, NodalSol* nsol);
        bool RemNsol(const int solver_id, const int dataset_id);
        bool RemAllNsol(const int solver_id);

        bool ContainsNsol(const int solver_id, const int dataset_id) const;

        NodalSol* GetNsol(const int solver_id, const int dataset_id) const;
        NsolMeshVis* GetSolVis(const int solver_id, const int dataset_id) const;

    private:
        struct NsolInfo_
        {
            NodalSol* Sol;
            NsolMeshVis* Vis;
        };

        Map<int, Map<int, NsolInfo_>> nsol_map_;
    };
}
