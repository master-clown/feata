#include "proj/glob_events.hpp"


namespace proj::glev
{
    Event<void(const String&, uint)> ProgramStatusChanged;
    Event<void()> TriangulationFinished;
    Event<void(MeshingStatus)> MeshingFinished;
    Event<void(SolvStatus)> SolvFinished;

    Event<void(const int, const int)> ProgressBarInitRequested;
    Event<void(const int)> ProgressBarChangeValueRequested;
    Event<void()> ProgressBarHideRequested;

    Event<void(const gui::wgt::BaseWidgetGeomSel* sender,
               const gui::SelectEntityType type)> GeomEntSelectionEnable;
    Event<void()> GeomEntSelectionDisable;
    Event<void(const List<geom::gid_t>& idx)> GeomEntSelectionUpdated;
    Event<void(const geom::gid_t)> GeomEntSelectedInLst;
    Event<void(const geom::gid_t id)> GeomEntUnselectOne;
    Event<void()> GeomEntUnselectAll;
    Event<void()> ActiveComponentBecameDirty;
}
