#pragma once

#include "iplugin.h"
#include "solv/solvinfo.hpp"
#include "solv/solvstatus.hpp"


/**
 *  WARNING!
 *
 * Solving process will proceed in separate thread.
 * Cancelation of it and access to the fields must be
 * implemented due to this fact, i.e. with possible
 * usage of multithread sync tools
 */
class SolverPlugin
        : public IPlugin
{
public:
    virtual void Solve() = 0;
    virtual void CancelSolving() = 0;

    virtual void SetSolvInfo(const SolvInfo& in) = 0;

    /**
     * \brief GetNodalDataSetList
     *     Nodal data is a map between nodes and reals. It's drawn as colored map
     *     on a shape.
     *
     *     This function returns a string which describes possible post-process data sets,
     *     which the plugin can provide with. The format is the following:
     *
     *         <dataset_id1>$<name1>$<desc1>~<dataset_id2>$<name2>$<desc2>~<...>
     *
     *         dataset_id: unique integer ID of this data set;
     *         name:       it's name (non-empty);
     *         desc:       it's description;
     *
     * ****WARNING! ID starts from 1!
     *
     * \sa 'GetNodalDataSet()'
     */
    virtual const char* GetNodalDataSetList() const = 0;


    /**
     * \brief GetNodalData
     *     Fills 'nodal_array' with values of nodal data set with ID 'dataset_id'.
     *
     * \returns
     *     Zero at success, non-zero otherwise.
     *
     *  WARNING! 'nodal_array' must be already allocated to contain as much doubles
     *           as there are nodes.
     */
    virtual int GetNodalDataSet(const int dataset_id, double* nodal_array) const = 0;

    virtual SolvStatus GetSolvStatus() const = 0;
    virtual const char* GetSolvStats() const = 0;
    virtual const char* GetSolvLog() const = 0;

    virtual bool IsCancelPossible() const = 0;  // can cancel solving?
};
