#pragma once


class MeshVS_Mesh;

struct MeshInfo;

namespace mesh
{
    class MeshVis
    {
    public:
        MeshVis();
        MeshVis(MeshInfo* info);
        virtual ~MeshVis();

        void Init(MeshInfo* info);
        void Clear();

        virtual MeshVS_Mesh* GetOCCMesh() const;

    private:
        struct MeshVisPriv;

        MeshVisPriv* priv_ = nullptr;
        MeshInfo* mesh_ = nullptr;
    };
}
