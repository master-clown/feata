#pragma once

#include <MeshVS_DataSource.hxx>


struct MeshInfo;

namespace mesh
{
    class MeshDataSource
            : public MeshVS_DataSource
    {
    public:
        MeshDataSource(const MeshInfo* mesh_info);
       ~MeshDataSource() override;

        Standard_Boolean GetGeom(const Standard_Integer ID,
                                 const Standard_Boolean IsElement,
                                 TColStd_Array1OfReal& Coords,
                                 Standard_Integer& NbNodes,
                                 MeshVS_EntityType& Type) const override;
        Standard_Boolean GetGeomType(const Standard_Integer ID,
                                     const Standard_Boolean IsElement,
                                     MeshVS_EntityType& Type) const override;
        Standard_Boolean Get3DGeom(const Standard_Integer ID,
                                   Standard_Integer& NbNodes,
                                   Handle(MeshVS_HArray1OfSequenceOfInteger)& Data) const override;
        Standard_Address GetAddr(const Standard_Integer ID,
                                 const Standard_Boolean IsElement) const override;
        Standard_Boolean GetNodesByElement(const Standard_Integer ID,
                                           TColStd_Array1OfInteger& NodeIDs,
                                           Standard_Integer& NbNodes) const override;
//        Standard_Boolean GetNormal(const Standard_Integer Id,
//                                   const Standard_Integer Max,
//                                   Standard_Real& nx,
//                                   Standard_Real& ny,
//                                   Standard_Real& nz) const override;
        const TColStd_PackedMapOfInteger& GetAllNodes() const override;
        const TColStd_PackedMapOfInteger& GetAllElements() const override;

    private:
        const MeshInfo* mesh_info_;
        TColStd_PackedMapOfInteger nodes_;
        TColStd_PackedMapOfInteger elems_;
    };
}
