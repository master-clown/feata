#pragma once

#include "geom/gid_t.hpp"
#include "gui/select_defs.hpp"
#include "mesh/meshingstatus.hpp"
#include "solv/solvstatus.hpp"
#include "stl/list.hpp"
#include "stl/string.hpp"
#include "util/event.hpp"


namespace gui::wgt { class BaseWidgetGeomSel; }

namespace proj::glev
{
    /**
     * \brief
     *     Triggered when there's need to change status bar message.
     *
     * \param msg:
     *     A new message.
     *
     * \param timeout:
     *     Timeout for the message, in msec.
     */
    extern Event<void(const String& msg, uint timeout)> ProgramStatusChanged;

    extern Event<void()> TriangulationFinished;
    extern Event<void(MeshingStatus)> MeshingFinished;
    extern Event<void(SolvStatus)> SolvFinished;

    /**
     * \brief ProgressBarInited
     *     Raised when a process is intended to report of its
     *     progress status. This shows a progress bar on the
     *     main window
     *
     * \param min: minimal progress value
     * \param max: maximal ...
     */
    extern Event<void(const int min, const int max)> ProgressBarInitRequested;

    /**
     * \brief ProgressBarInited
     *     Raised when a process wants to change its progress
     *     value. \sa ProgressBarInited()
     *
     * \param val: new progress bar value
     */
    extern Event<void(const int val)> ProgressBarChangeValueRequested;
    extern Event<void()> ProgressBarHideRequested;

    extern Event<void(const gui::wgt::BaseWidgetGeomSel* sender,
                      const gui::SelectEntityType type)> GeomEntSelectionEnable;
    extern Event<void()> GeomEntSelectionDisable;

    /**
     * \brief GeomEntSelectionUpdated
     *     Raised when the user (un-) selects a geometric entity
     *     in the viewport
     *
     * \param id_lst: list of IDs of currently selected entities
     */
    extern Event<void(const List<geom::gid_t>& id_lst)> GeomEntSelectionUpdated;

    /**
     * \brief GeomEntSelectedInLst, GeomEntUnselectedInLst
     *     Raised when the user (un-) selects a geometric entity
     *     in the list of currently selected entities. Serves for
     *     distinction of desired entity among the uniformly colored
     *     selection
     *
     * \param id: chosen entity's ID
     */
    extern Event<void(const geom::gid_t id)> GeomEntSelectedInLst;

    extern Event<void(const geom::gid_t id)> GeomEntUnselectOne;
    extern Event<void()> GeomEntUnselectAll;

    extern Event<void()> ActiveComponentBecameDirty;
}
