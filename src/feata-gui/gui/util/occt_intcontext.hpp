#pragma once

#include <AIS_InteractiveContext.hxx>
#include <Standard_Handle.hxx>


/**
 * \brief The OcctInteractiveContext class
 *     Used to enhance selection possibilities.
 */
class OcctInteractiveContext
        : public AIS_InteractiveContext
{
public:
    OcctInteractiveContext() = default;
    OcctInteractiveContext(const Handle(V3d_Viewer)& viewer);
   ~OcctInteractiveContext() override = default;

    /**
     * \brief RectSelectInit
     *     Initialises the rectangular selection. Must be called before any 'RectSelect...()' routine.
     */
    void RectSelectInit();

    /**
     * \brief RectSelectAdd
     *     Adds new detected entities into the picked list of canditates for selection in addition to the already selected.
     *     Removes the detected entities from prevously selected candidates.
     */
    AIS_StatusOfPick RectSelectAdd(const Standard_Integer  theXPMin,
                                   const Standard_Integer  theYPMin,
                                   const Standard_Integer  theXPMax,
                                   const Standard_Integer  theYPMax,
                                   const Handle(V3d_View)& theView,
                                   const Standard_Boolean  theToUpdateViewer);

    /**
     * \brief RectSelectAdd
     *     Removes the detected entities from prevously selected candidates.
     */
    AIS_StatusOfPick RectSelectRemove(const Standard_Integer  theXPMin,
                                      const Standard_Integer  theYPMin,
                                      const Standard_Integer  theXPMax,
                                      const Standard_Integer  theYPMax,
                                      const Handle(V3d_View)& theView,
                                      const Standard_Boolean  theToUpdateViewer);

    /**
     * \brief RectSelectFinish
     *     Updates the main selection with or without candidates, generated by 'AddRectSelect...()' or 'RemoveRect...()'.
     */
    void RectSelectFinish();

protected:
    AIS_NListOfEntityOwner myCurrentChangingSelection;
    bool is_rect_sel_active_ = false;

    using Base = AIS_InteractiveContext;
};
