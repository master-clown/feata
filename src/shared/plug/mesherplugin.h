#pragma once

#include "iplugin.h"
#include "mesh/meshinfo.hpp"
#include "mesh/meshingstatus.hpp"


/**
 *  WARNING!
 *
 * Meshing process will proceed in separate thread.
 * Cancelation of it and access to the fields must be
 * implemented due to this fact, i.e. with possible
 * usage of multithread sync tools
 */
class MesherPlugin
        : public IPlugin
{
public:
    virtual void Mesh() = 0;
    virtual void CancelMeshing() = 0;

    virtual void SetInputMeshInfo(const MeshInfo& in) = 0;
    virtual void SetOutputMeshInfo(MeshInfo& in) = 0;

    /**
     * \brief FreeOutputMeshInfo
     *     If fields of 'in' were allocated in this plugin,
     *     (more precisely, library module), they must be
     *     deallocated in here either.
     */
    virtual void FreeOutputMeshInfo(MeshInfo& in) = 0;

    virtual MeshingStatus GetMeshingStatus() const = 0;
    virtual const char* GetMeshStats() const = 0;
    virtual const char* GetMesherLog() const = 0;

    virtual bool IsCancelPossible() const = 0;  // can cancel meshing?
};
