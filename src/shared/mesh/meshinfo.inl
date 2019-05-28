#pragma once


inline MeshInfo::Polygon::Polygon() : VertexLst(0), VertexNum(0) {}
inline MeshInfo::Facet::Facet()     : PolyLst(0), HoleLst(0), PolyNum(0), HoleNum(0) {}
inline MeshInfo::MeshInfo()            { Nullify(); }

inline MeshInfo::Polygon::~Polygon()   { Dealloc(); }
inline MeshInfo::Facet::~Facet()       { Dealloc(); }
inline MeshInfo::~MeshInfo()           { Dealloc(); }

inline void MeshInfo::Polygon::Dealloc()
{
    if(VertexLst) delete[] VertexLst;
}

inline void MeshInfo::Facet::Dealloc()
{
    if(PolyLst) delete[] PolyLst;
    if(HoleLst) delete[] HoleLst;
}

inline void MeshInfo::Dealloc()
{
    if(NodeLst)
        delete[] NodeLst;
    if(NodeMarkerLst)
        delete[] NodeMarkerLst;
    if(NodeAttribLst)
        delete[] NodeAttribLst;
    if(ElemLst)
        delete[] ElemLst;
    if(ElemRegionLst)
        delete[] ElemRegionLst;
    if(ElemAttribLst)
        delete[] ElemAttribLst;
    if(FacetLst)
        delete[] FacetLst;
    if(FacetMarkerLst)
        delete[] FacetMarkerLst;
    if(RegionLst)
        delete[] RegionLst;
    if(ElemFaceLst)
        delete[] ElemFaceLst;
    if(ElemFaceMarkerLst)
        delete[] ElemFaceMarkerLst;
    if(EdgeLst)
        delete[] EdgeLst;
    if(EdgeMarkerLst)
        delete[] EdgeMarkerLst;
    if(HoleLst)
        delete[] HoleLst;
    if(VertexLocalIdxLst)
        delete[] VertexLocalIdxLst;
    if(ElemFaceVertexLocalIdxLst)
        delete[] ElemFaceVertexLocalIdxLst;
    if(ElemFaceVertexCountLst)
        delete[] ElemFaceVertexCountLst;
}

inline void MeshInfo::Nullify()
{
    char* p = reinterpret_cast<char*>(this);
    for(size_t s = 0; s < sizeof(*this); ++s)
        p[s] = 0;
}
