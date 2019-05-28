#pragma once


struct MeshInfo;

/**
 *  All important fields will be marked with "[i][o]!".
 *  i -- important for input, o -- for output
 */
struct SolvInfo
{
    typedef double num_t;

    const MeshInfo* Mesh;   /*i!*/

    /**
     * \brief DispLst
     *     Displacement vector list for every node. Three numbers per node.
     */
    num_t* DispLst;         /*o!*/
#pragma message("Think where dealloc it")


    SolvInfo();
   ~SolvInfo();

    void Dealloc();
    void Nullify();
};


#include "solv/solvinfo.inl"
