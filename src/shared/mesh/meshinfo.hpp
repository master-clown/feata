#pragma once


/** All important fields will be marked with "[i][o]!".
 *  i -- important for input, o -- for output
 *
 *  General way of describing mesh is taken from here:
 *     http://wias-berlin.de/software/tetgen/1.5/doc/manual/manual007.html
 *
 *  (or 'tetgenio' struct from TetGen library)
 */
struct MeshInfo
{
    typedef double num_t;
    typedef int id_t;
    typedef unsigned long siz_t;

    struct Polygon
    {
        id_t* VertexLst;
        siz_t VertexNum;

        inline Polygon();
        inline ~Polygon();

        inline void Dealloc();
    };

    struct Facet
    {
        Polygon* PolyLst;
        num_t* HoleLst;

        siz_t  PolyNum;
        siz_t  HoleNum;

        inline Facet();
        inline ~Facet();

        inline void Dealloc();
    };

    using mrk_t = int;

    num_t* NodeLst;          /*io!*/      // 3 per node
    mrk_t* NodeMarkerLst;                 // NodeNum count
    num_t* NodeAttribLst;
    id_t*  ElemLst;          /*o!*/
    id_t*  ElemRegionLst;    /*o!*/
    num_t* ElemAttribLst;
    Facet* FacetLst;         /*i!*/
    mrk_t* FacetMarkerLst;   /*i!*/
    num_t* RegionLst;        /*i!*/       // 3 coords of internal point, 1 region id

#pragma message("It'd've been much-much better, if every face was described by two numbers: elem idx, face local idx")
    id_t*  ElemFaceLst;      /*o!*/       // boundary only
    mrk_t* ElemFaceMarkerLst;/*o!*/       //

    id_t*  EdgeLst;          /*o!*/       // [0, 1] -- endpoint nodes
    mrk_t* EdgeMarkerLst;    /*o!*/       // ???
    num_t* HoleLst;

/** ***
     * \brief VertexLocalIdxLst
     *     Local indices of geometrical vertices of the element (not e.g. node on the middle of an edge).
     *
     *  \sa 'VertexLocalIdxLstSize'
     */
    id_t*  VertexLocalIdxLst;/*o!*/

/** ***
     * \brief ElemFaceLocalIdxLst
     *     List of local indices of face corners in an element. Used to access any face of any element
     *     through 'ElemLst'. For instance, if the element is tetrahedron with local vertex
     *     indices { 0, 1, 2, 3 } (i.e. the first vertex of the tetrahedron is 0th index in 'ElemLst' (with
     *     correct offset), the second is the 1st, and so on), then vertices of faces of the tetrahedron
     *     can be described using those local indices. Here, the faces are triangles, so we need 3 vertices
     *     per face. There are 4 triangles, so the description might be as such (if written continiously):
     *
     *         { 0, 2, 1,  1, 2, 3,  0, 3, 2,  0, 1, 3 }    <-- local vertex index
     *          |  0th  |-|  1st  |-|  2nd  |-|  3rd  |     <-- face index
     *
     *     The order of indices per face must be counterclockwise, so that external normals could be calculated by
     *     them. Otherwise mesh rendering would not go properly.
     *
     *     This list is used with 'ElemFaceVertexCountLst', to be adapted for a general case of polyhedral
     *     element, when faces can have different number of verices.
     *
     ******WARNING! THE SIZE OF THIS LIST MUST BE the sum of elements of 'ElemFaceVertexCountLst'
     *
     * \sa 'mesh/meshdatasource.cpp':'MeshDataSource::Get3DGeom()' to see, how these lists are used,
     *     to get the idea fully.
     */
    id_t* ElemFaceVertexLocalIdxLst; /*o!*/

/** ***
     * \brief ElemFaceVertexCountLst
     *     Number of vertices per face. Used with 'ElemFaceLocalIdxLst'. In terms of example for that field,
     *     this will look as such:
     *
     *     {    3,        3         3         3    }    <-- number of vertices for i-th face
     *      |  0th  |-|  1st  |-|  2nd  |-|  3rd  |     <-- face index
     *
     ******WARNING! THE SIZE OF THIS LIST MUST BE the 'NumFacePerElem'
     */
    siz_t* ElemFaceVertexCountLst; /*o!*/

    siz_t NodeNum;              /*io!*/
    siz_t NodeAttribPerObj;
    siz_t ElemNum;              /*o!*/
    siz_t ElemAttribPerObj;
    siz_t FacetNum;             /*i!*/
    siz_t RegionNum;            /*i!*/
    siz_t ElemFaceNum;          /*o!*/
    siz_t EdgeNum;              /*o!*/
    siz_t HoleNum;

    siz_t NumNodePerElem;       /*o!*/
    siz_t NumFacePerElem;       /*o!*/
    siz_t VertexLocalIdxLstSize;/*o!*/  // number of vertices in elem at the same time


    MeshInfo();
   ~MeshInfo();

    void Dealloc();
    void Nullify();
};


#include "mesh/meshinfo.inl"
