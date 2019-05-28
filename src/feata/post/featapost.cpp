#include "post/featapost.hpp"
#include "post/nodalsol.hpp"
#include "post/nsolmeshvis.hpp"


namespace post
{
     FeataPost::~FeataPost()
     {
         for(auto& smap: nsol_map_)
             for(auto& sol: smap)
             {
                 delete sol.Sol;
                 delete sol.Vis;
             }

     }

     bool FeataPost::RemAllSol(const int solver_id)
     {
         if(!RemAllNsol(solver_id)) return false;

         return true;
     }

     bool FeataPost::RemAllNsol(const int solver_id)
     {
         if(!nsol_map_.contains(solver_id)) return false;

         const auto ds_lst = nsol_map_[solver_id].keys();
         for(const auto dsid: ds_lst)
             RemNsol(solver_id, dsid);

         return true;
     }

     bool FeataPost::AddNsol(const int solver_id, const int dataset_id, NodalSol* nsol)
     {
         if(ContainsNsol(solver_id, dataset_id))
             RemNsol(solver_id, dataset_id);

         auto vs = new NsolMeshVis(nsol);

         nsol_map_[solver_id][dataset_id].Sol = nsol;
         nsol_map_[solver_id][dataset_id].Vis = vs;

         return true;
     }

     bool FeataPost::RemNsol(const int solver_id, const int dataset_id)
     {
         if(ContainsNsol(solver_id, dataset_id))
         {
             delete nsol_map_[solver_id][dataset_id].Sol;
             delete nsol_map_[solver_id][dataset_id].Vis;
             nsol_map_[solver_id].remove(dataset_id);
             if(nsol_map_[solver_id].empty())
                 nsol_map_.remove(solver_id);
         }

         return false;
     }

     bool FeataPost::ContainsNsol(const int solver_id, const int dataset_id) const
     {
         return nsol_map_.contains(solver_id) && nsol_map_[solver_id].contains(dataset_id);
     }

     NodalSol* FeataPost::GetNsol(const int solver_id, const int dataset_id) const
     {
         return ContainsNsol(solver_id, dataset_id) ? nsol_map_[solver_id][dataset_id].Sol : nullptr;
     }

     NsolMeshVis* FeataPost::GetSolVis(const int solver_id, const int dataset_id) const
     {
         return ContainsNsol(solver_id, dataset_id) ? nsol_map_[solver_id][dataset_id].Vis : nullptr;
     }
}
