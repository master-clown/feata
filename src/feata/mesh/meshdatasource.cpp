#include "mesh/meshdatasource.hpp"
#include <MeshVS_Buffer.hxx>
#include "mesh/meshinfo.hpp"


namespace mesh
{
    MeshDataSource::MeshDataSource(const MeshInfo* mesh_info)
        : mesh_info_(mesh_info)
    {
        for(unsigned i = 1; i <= mesh_info->NodeNum; ++i) nodes_.Add(i);
        for(unsigned i = 1; i <= mesh_info->ElemNum; ++i) elems_.Add(i);
    }

    MeshDataSource::~MeshDataSource() {}

    Standard_Boolean MeshDataSource::GetGeom(const Standard_Integer ID,
                                             const Standard_Boolean IsElement,
                                             TColStd_Array1OfReal& Coords,
                                             Standard_Integer& NbNodes,
                                             MeshVS_EntityType& Type) const
    {
        if(!mesh_info_)
            return false;

        if(IsElement)
        {
            if(ID < 1 || (int)mesh_info_->ElemNum < ID)
                return false;

            Type = MeshVS_EntityType(MeshVS_ET_Volume);
            NbNodes = mesh_info_->NumNodePerElem;

            for(int i = 0, k = 1; i < NbNodes; ++i)
            {
                const int i_node = mesh_info_->ElemLst[NbNodes*(ID - 1) + i] - 1;
                for(int j = 0; j < 3; ++j, ++k)
                    Coords(k) = mesh_info_->NodeLst[3*i_node + j];
            }

            return true;
        }

        if(ID < 1 || (int)mesh_info_->NodeNum < ID)
            return false;

        Type = MeshVS_ET_Node;
        NbNodes = 1;

        Coords(1) = mesh_info_->NodeLst[ID - 1 + 0];
        Coords(2) = mesh_info_->NodeLst[ID - 1 + 1];
        Coords(3) = mesh_info_->NodeLst[ID - 1 + 2];

        return true;
    }

    Standard_Boolean MeshDataSource::GetGeomType(const Standard_Integer /*ID*/,
                                                 const Standard_Boolean IsElement,
                                                 MeshVS_EntityType& Type) const
    {
        Type = IsElement ? MeshVS_EntityType(MeshVS_ET_Volume) : MeshVS_ET_Node;
        return true;
    }

    Standard_Boolean MeshDataSource::Get3DGeom(const Standard_Integer ID,
                                               Standard_Integer& NbNodes,
                                               Handle(MeshVS_HArray1OfSequenceOfInteger)& Data) const
    {
        if(!mesh_info_ || !mesh_info_->ElemFaceVertexCountLst ||
           !mesh_info_->ElemFaceVertexLocalIdxLst || !mesh_info_->NumFacePerElem)
            return false;
        if(ID < 1 || (int)mesh_info_->ElemNum < ID)
            return false;

        Data = new MeshVS_HArray1OfSequenceOfInteger(1, mesh_info_->NumFacePerElem);
        NbNodes = (int)mesh_info_->NumNodePerElem;

        for(auto i_face = 1u, i_vert = 0u; i_face <= mesh_info_->NumFacePerElem; ++i_face)
        {
            for(auto i = 0u; i < mesh_info_->ElemFaceVertexCountLst[i_face-1]; ++i, ++i_vert)
            {
                Data->ChangeValue(i_face).Append(mesh_info_->ElemFaceVertexLocalIdxLst[i_vert]);
            }
        }

        return true;
    }

    Standard_Address MeshDataSource::GetAddr(const Standard_Integer /*ID*/,
                                             const Standard_Boolean /*IsElement*/) const
    {
        return nullptr;
    }

    Standard_Boolean MeshDataSource::GetNodesByElement(const Standard_Integer ID,
                                                       TColStd_Array1OfInteger& NodeIDs,
                                                       Standard_Integer& NbNodes) const
    {
        if(!mesh_info_)
            return false;
        if(ID < 1 || (int)mesh_info_->ElemNum < ID)
            return false;

        NbNodes = (int)mesh_info_->NumNodePerElem;

        for(int k = 0; k < NbNodes; ++k)
            NodeIDs(1 + k) = mesh_info_->ElemLst[NbNodes*(ID - 1) + k];

        return Standard_True;
    }

//    Standard_Boolean MeshDataSource::GetNormal(const Standard_Integer Id,
//                                               const Standard_Integer Max,
//                                               Standard_Real& nx,
//                                               Standard_Real& ny,
//                                               Standard_Real& nz) const
//    {
//        if(Max <= 0)
//            return Standard_False;

//        MeshVS_Buffer aCoordsBuf (3*Max*sizeof(Standard_Real));
//        TColStd_Array1OfReal Coords ( aCoordsBuf, 1, 3*Max );
//        Standard_Integer nbNodes;
//        MeshVS_EntityType Type;

//        Standard_Boolean res = Standard_False;

//        if(!GetGeom ( Id, Standard_True, Coords, nbNodes, Type ))
//            return res;

//        if(nbNodes >= 3)
//        {
//            Standard_Real x1 = Coords( 1 );
//            Standard_Real y1 = Coords( 2 );
//            Standard_Real z1 = Coords( 3 );
//            Standard_Real x2 = Coords( 4 );
//            Standard_Real y2 = Coords( 5 );
//            Standard_Real z2 = Coords( 6 );
//            Standard_Real x3 = Coords( ( nbNodes - 1 ) * 3 + 1 );
//            Standard_Real y3 = Coords( ( nbNodes - 1 ) * 3 + 2 );
//            Standard_Real z3 = Coords( ( nbNodes - 1 ) * 3 + 3 );
//            Standard_Real p1 = x2 - x1, p2 = y2 - y1, p3 = z2 - z1,
//                    q1 = x3 - x1, q2 = y3 - y1, q3 = z3 - z1;
//            nx = p2*q3 - p3*q2;
//            ny = p3*q1 - p1*q3;
//            nz = p1*q2 - p2*q1;
//            Standard_Real len = sqrt ( nx*nx + ny*ny + nz*nz );
//            if ( len <= gp::Resolution() )
//            {
//                nx = ny = nz = 0;
//                return res;
//            }
//            nx /= len; ny /= len; nz /= len;
//            res = Standard_True;
//        }
//        return res;
//    }

    const TColStd_PackedMapOfInteger& MeshDataSource::GetAllNodes() const
    {
        return nodes_;
    }

    const TColStd_PackedMapOfInteger& MeshDataSource::GetAllElements() const
    {
        return elems_;
    }
}
