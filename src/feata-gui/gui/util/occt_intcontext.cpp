#include "gui/util/occt_intcontext.hpp"
#include <V3d_View.hxx>


OcctInteractiveContext::OcctInteractiveContext(const Handle(V3d_Viewer)& viewer)
    : Base(viewer)
{}

void OcctInteractiveContext::RectSelectInit()
{
    myCurrentChangingSelection.Clear();
    for (AIS_NListOfEntityOwner::Iterator aSelIter(mySelection->Objects()); aSelIter.More(); aSelIter.Next())
        myCurrentChangingSelection.Append(aSelIter.Value());

    is_rect_sel_active_ = true;
}

//=======================================================================
//function : AddRectSelect
//purpose  :
//=======================================================================
AIS_StatusOfPick OcctInteractiveContext::RectSelectAdd(const Standard_Integer theXPMin,
                                                       const Standard_Integer theYPMin,
                                                       const Standard_Integer theXPMax,
                                                       const Standard_Integer theYPMax,
                                                       const Handle(V3d_View)& theView,
                                                       const Standard_Boolean toUpdateViewer)
{
    if (!is_rect_sel_active_)
        return AIS_SOP_NothingSelected;

    if (theView->Viewer() != myMainVwr)
    {
        throw Standard_ProgramError("OcctInteractiveContext::ShiftSelect() - invalid argument");
    }

    if (HasOpenedContext())
    {
        return myLocalContexts(myCurLocalIndex)->ShiftSelect(theXPMin, theYPMin, theXPMax, theYPMax,
                                                             theView, toUpdateViewer);
    }

    UnhilightSelected(Standard_False);

    mySelection->Clear();
    for (const auto& owner : myCurrentChangingSelection)
        mySelection->Select(owner);

    myWasLastMain = Standard_True;
    myMainSel->Pick(theXPMin, theYPMin, theXPMax, theYPMax, theView);
    for (Standard_Integer aPickIter = 1; aPickIter <= myMainSel->NbPicked(); ++aPickIter)
    {
        const Handle(SelectMgr_EntityOwner) anOwner = myMainSel->Picked(aPickIter);
        if (anOwner.IsNull() || !anOwner->HasSelectable() || !myFilters->IsOk(anOwner)
            || mySelection->IsSelected(anOwner))
            continue;

        AIS_SelectStatus aSelStatus = mySelection->Select(anOwner);
        anOwner->SetSelected(aSelStatus == AIS_SS_Added);
    }

    HilightSelected(toUpdateViewer);

    Standard_Integer aSelNum = NbSelected();

    return (aSelNum == 0) ? AIS_SOP_NothingSelected
                          : (aSelNum == 1) ? AIS_SOP_OneSelected
                                           : AIS_SOP_SeveralSelected;
}

//=======================================================================
//function : RemoveRectSelect
//purpose  :
//=======================================================================
AIS_StatusOfPick OcctInteractiveContext::RectSelectRemove(const Standard_Integer theXPMin,
                                                          const Standard_Integer theYPMin,
                                                          const Standard_Integer theXPMax,
                                                          const Standard_Integer theYPMax,
                                                          const Handle(V3d_View)& theView,
                                                          const Standard_Boolean toUpdateViewer)
{
    if (!is_rect_sel_active_)
        return AIS_SOP_NothingSelected;

    if (theView->Viewer() != myMainVwr)
    {
        throw Standard_ProgramError("OcctInteractiveContext::ShiftSelect() - invalid argument");
    }

    if (HasOpenedContext())
    {
        return myLocalContexts(myCurLocalIndex)->ShiftSelect(theXPMin, theYPMin, theXPMax, theYPMax,
                                                             theView, toUpdateViewer);
    }

    UnhilightSelected(Standard_False);

    mySelection->Clear();
    for (const auto& owner : myCurrentChangingSelection)
        mySelection->Select(owner);

    myWasLastMain = Standard_True;
    myMainSel->Pick(theXPMin, theYPMin, theXPMax, theYPMax, theView);
    for (Standard_Integer aPickIter = 1; aPickIter <= myMainSel->NbPicked(); ++aPickIter)
    {
        const Handle(SelectMgr_EntityOwner) anOwner = myMainSel->Picked(aPickIter);
        if (anOwner.IsNull() || !anOwner->HasSelectable() || !myFilters->IsOk(anOwner)
            || !mySelection->IsSelected(anOwner))
            continue;

        AIS_SelectStatus aSelStatus = mySelection->Select(anOwner);
        anOwner->SetSelected(aSelStatus == AIS_SS_Added);
    }

    HilightSelected(toUpdateViewer);

    Standard_Integer aSelNum = NbSelected();

    return (aSelNum == 0) ? AIS_SOP_NothingSelected
                          : (aSelNum == 1) ? AIS_SOP_OneSelected
                                           : AIS_SOP_SeveralSelected;
}

//! Rectangle of selection; adds new detected entities into the picked list in
//! addition to the previously detected.
void OcctInteractiveContext::RectSelectFinish()
{
    myCurrentChangingSelection.Clear();
    is_rect_sel_active_ = false;
}
