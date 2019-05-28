#pragma once

#include "mesh/meshvis.hpp"


namespace post { struct NodalSol; }

namespace post
{
    // nodal solution visualizator
    class NsolMeshVis
            : public mesh::MeshVis
    {
    public:
        NsolMeshVis();
        NsolMeshVis(NodalSol* info);
       ~NsolMeshVis() override;

        void Init(NodalSol* info);
        void Clear();

        MeshVS_Mesh* GetOCCMesh() const override;

        double GetMinResValue() const;
        double GetMaxResValue() const;

    private:
        struct NsolMeshVisPriv;

        NsolMeshVisPriv* priv_ = nullptr;
        NodalSol* sol_ = nullptr;
    };
}
