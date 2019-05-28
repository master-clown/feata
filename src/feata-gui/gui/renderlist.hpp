#pragma once


namespace geom { class GeomVis; }
namespace mesh { class MeshVis; }
namespace post { class NsolMeshVis; }

namespace gui
{
    class RenderList
    {
    public:
        RenderList();
       ~RenderList();

        void SetCurrentGeometry(geom::GeomVis* g);
        void SetCurrentTrianGeometry(geom::GeomVis* g);
        void SetCurrentMesh(mesh::MeshVis* m);
        void SetCurrentNodalSol(const int solver_id, const int dataset_id, post::NsolMeshVis* s);

        bool RemoveGeometry(geom::GeomVis* g, bool* was_current = nullptr);
        bool RemoveTrianGeometry(geom::GeomVis* g, bool* was_current = nullptr);
        bool RemoveMesh(mesh::MeshVis* m, bool* was_current = nullptr);
        bool RemoveNodalSol(const int solver_id, const int dataset_id, bool* was_current = nullptr);

        geom::GeomVis* GetCurrentGeometry() const;
        geom::GeomVis* GetCurrentTrianGeometry() const;
        mesh::MeshVis* GetCurrentMesh() const;
        post::NsolMeshVis* GetCurrentNodalSol() const;

    private:
        struct RenderListPriv;

        RenderListPriv* priv_;
    };
}
