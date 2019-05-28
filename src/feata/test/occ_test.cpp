#include "test/test.hpp"
#include <gp_Pnt.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeSegment.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <gp_Trsf.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopoDS.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepTools.hxx>
#include <Geom_Plane.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <BRepLib.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>

#define WIDTH 50
#define HEIGHT 70
#define THICK 30


namespace test
{
    void OccTest(const util::CmdArgList& arg_lst)
    {
        UNUSED_PARAM(arg_lst);

        gp_Pnt aPnt1(-WIDTH / 2., 0, 0);
        gp_Pnt aPnt2(-WIDTH / 2., -THICK / 4., 0);
        gp_Pnt aPnt3(0, -THICK / 2., 0);
        gp_Pnt aPnt4(WIDTH / 2., -THICK / 4., 0);
        gp_Pnt aPnt5(WIDTH / 2., 0, 0);

        Handle(Geom_TrimmedCurve) aArcOfCircle = GC_MakeArcOfCircle(aPnt2,aPnt3,aPnt4);
        Handle(Geom_TrimmedCurve) aSegment1    = GC_MakeSegment(aPnt1, aPnt2);
        Handle(Geom_TrimmedCurve) aSegment2    = GC_MakeSegment(aPnt4, aPnt5);
        TopoDS_Edge aEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
        TopoDS_Edge aEdge2 = BRepBuilderAPI_MakeEdge(aArcOfCircle);
        TopoDS_Edge aEdge3 = BRepBuilderAPI_MakeEdge(aSegment2);
        TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(aEdge1, aEdge2, aEdge3);

        gp_Trsf aTrsf;
        aTrsf.SetMirror(gp::OX());

        BRepBuilderAPI_Transform aBRepTrsf(aWire, aTrsf);
        auto aMirroredWire = TopoDS::Wire(aBRepTrsf.Shape());
        BRepBuilderAPI_MakeWire mkWire;
        mkWire.Add(aWire);
        mkWire.Add(aMirroredWire);
        TopoDS_Wire myWireProfile = mkWire.Wire();

        TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(myWireProfile);
        TopoDS_Shape myBody = BRepPrimAPI_MakePrism(myFaceProfile, gp_Vec(0,0,HEIGHT));

        BRepFilletAPI_MakeFillet mkFillet(myBody);
        TopExp_Explorer anEdgeExplorer(myBody, TopAbs_EDGE);
        while(anEdgeExplorer.More())
        {
            TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExplorer.Current());
            mkFillet.Add(THICK / 12.0, anEdge);

            anEdgeExplorer.Next();
        }

        myBody = mkFillet.Shape();

        gp_Pnt neckLocation(0, 0, HEIGHT);
        gp_Ax2 neckAx2(neckLocation, gp::DZ());
        auto myNeckRadius = THICK / 4.0;
        auto myNeckHeight = HEIGHT / 10.0;
        BRepPrimAPI_MakeCylinder MKCylinder(neckAx2, myNeckRadius, myNeckHeight);
        TopoDS_Shape myNeck = MKCylinder.Shape();

        myBody = BRepAlgoAPI_Fuse(myBody, myNeck);

        TopoDS_Face faceToRemove;
        Standard_Real zMax = -1;
        TopTools_ListOfShape facesToRemove;
        for(TopExp_Explorer aFaceExplorer(myBody, TopAbs_FACE) ; aFaceExplorer.More() ; aFaceExplorer.Next())
        {
            TopoDS_Face aFace = TopoDS::Face(aFaceExplorer.Current());
            Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);
            if(aSurface->DynamicType() == STANDARD_TYPE(Geom_Plane))
            {
                Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aSurface);
                gp_Pnt aPnt = aPlane->Location();
                Standard_Real aZ = aPnt.Z();
                if(aZ > zMax)
                {
                    zMax = aZ;
                    faceToRemove = aFace;
                }
            }
        }
        facesToRemove.Append(faceToRemove);

        BRepOffsetAPI_MakeThickSolid BodyMaker;
        BodyMaker.MakeThickSolidByJoin(myBody, facesToRemove, -THICK / 50.0, 1.e-3);
        myBody = BodyMaker.Shape();

        Handle(Geom_CylindricalSurface) aCyl1 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 0.99);
        Handle(Geom_CylindricalSurface) aCyl2 = new Geom_CylindricalSurface(neckAx2, myNeckRadius * 1.05);

        gp_Pnt2d aPnt(2. * M_PI, myNeckHeight / 2.);
        gp_Dir2d aDir(2. * M_PI, myNeckHeight / 4.);
        gp_Ax2d anAx2d(aPnt, aDir);

        Standard_Real aMajor = 2. * M_PI;
        Standard_Real aMinor = myNeckHeight / 10;
        Handle(Geom2d_Ellipse) anEllipse1 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor);
        Handle(Geom2d_Ellipse) anEllipse2 = new Geom2d_Ellipse(anAx2d, aMajor, aMinor / 4);
        Handle(Geom2d_TrimmedCurve) anArc1 = new Geom2d_TrimmedCurve(anEllipse1, 0, M_PI);
        Handle(Geom2d_TrimmedCurve) anArc2 = new Geom2d_TrimmedCurve(anEllipse2, 0, M_PI);
        gp_Pnt2d anEllipsePnt1 = anEllipse1->Value(0);
        gp_Pnt2d anEllipsePnt2;
        anEllipse1->D0(M_PI, anEllipsePnt2);
        Handle(Geom2d_TrimmedCurve) aSegment = GCE2d_MakeSegment(anEllipsePnt1, anEllipsePnt2);

        TopoDS_Edge anEdge1OnSurf1 = BRepBuilderAPI_MakeEdge(anArc1, aCyl1);
        TopoDS_Edge anEdge2OnSurf1 = BRepBuilderAPI_MakeEdge(aSegment, aCyl1);
        TopoDS_Edge anEdge1OnSurf2 = BRepBuilderAPI_MakeEdge(anArc2, aCyl2);
        TopoDS_Edge anEdge2OnSurf2 = BRepBuilderAPI_MakeEdge(aSegment, aCyl2);
        TopoDS_Wire threadingWire1 = BRepBuilderAPI_MakeWire(anEdge1OnSurf1, anEdge2OnSurf1);
        TopoDS_Wire threadingWire2 = BRepBuilderAPI_MakeWire(anEdge1OnSurf2, anEdge2OnSurf2);
        BRepLib::BuildCurves3d(threadingWire1);
        BRepLib::BuildCurves3d(threadingWire2);

        BRepOffsetAPI_ThruSections aTool(Standard_True);
        aTool.AddWire(threadingWire1); aTool.AddWire(threadingWire2);
        aTool.CheckCompatibility(Standard_False);
        TopoDS_Shape myThreading = aTool.Shape();

        TopoDS_Compound aRes;
        BRep_Builder aBuilder;
        aBuilder.MakeCompound (aRes);
        aBuilder.Add (aRes, myBody);
        aBuilder.Add (aRes, myThreading);

        BRepTools::Write(aRes, "shape.brep");
    }
}
