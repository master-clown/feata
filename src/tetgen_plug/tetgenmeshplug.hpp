#pragma once

#include <atomic>
#include <map>
#include "plug/mesherplugin.h"


enum TetGenErrorCode
{
    TET_GEN_ECODE_OUT_OF_MEMORY = 1,
    TET_GEN_ECODE_INTERNAL_ERROR = 2,
    TET_GEN_ECODE_SELF_INTERSECT = 3,
    TET_GEN_ECODE_BAD_GEOM_PRECISION = 4,
    TET_GEN_ECODE_CLOSE_FACETS = 5,
    TET_GEN_ECODE_INPUT_ERROR = 10
};
const char* TetGenErrorCodeToString(const TetGenErrorCode code);


class tetgenio;

class TetgenMeshPlug
        : public MesherPlugin
{
public:
    TetgenMeshPlug();
   ~TetgenMeshPlug() override;

    int Init(const ServiceInfo service_lst[],
             const unsigned int service_num) override;

    void Mesh() override;
    void CancelMeshing() override;

    void SetInputMeshInfo(const MeshInfo& in) override;
    void SetOutputMeshInfo(MeshInfo& out) override;
    void FreeOutputMeshInfo(MeshInfo& out) override;

    MeshingStatus GetMeshingStatus() const override;
    const char* GetMeshStats() const override;
    const char* GetMesherLog() const override;

    const char* ListRequiredSettings() const override;

    bool IsCancelPossible() const override;

protected:
    virtual int CallService(const char* service_name,
                            const char* service_args,
                            const unsigned int arg_size = 0,
                            char** result = nullptr,
                            unsigned int* res_size = nullptr) override;

private:
    const ServiceInfo* serv_lst_;
    unsigned int serv_num_;
    const MeshInfo* mesh_in;
    MeshInfo* mesh_out;

    std::atomic<MeshingStatus> status_;
};

DECL_CREATE_FREE_PLUG_ROUTINES
