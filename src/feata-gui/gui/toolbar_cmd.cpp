#include "gui/toolbar_cmd.hpp"


namespace gui::cmd
{
    CameraView ComposeCameraView(CameraView v1, CameraView v2)
    {
        bool is_greater = false;
        if(v1 == v2 && v1 == CAMERA_VIEW_FREE) return CAMERA_VIEW_FREE;
        if(v1 > v2) { is_greater = true; CameraView tmp = v1; v1 = v2; v2 = tmp; }

        if(v1 == CAMERA_VIEW_FREE)
        {
            if(v2 <= CAMERA_VIEW_XY_270) return CAMERA_VIEW_XY;
            if(v2 <= CAMERA_VIEW_YZ_270) return CAMERA_VIEW_YZ;
            if(v2 <= CAMERA_VIEW_XZ_270) return CAMERA_VIEW_XZ;
                                    else return CAMERA_VIEW_XYZ;
        }
        if(CAMERA_VIEW_XY <= v1 && v2 <= CAMERA_VIEW_XY_270)
            return CameraView(CAMERA_VIEW_XY + (1 + v1 + v2 - 2*CAMERA_VIEW_XY) % 4);
        if(CAMERA_VIEW_YZ <= v1 && v2 <= CAMERA_VIEW_YZ_270)
            return CameraView(CAMERA_VIEW_YZ + (1 + v1 + v2 - 2*CAMERA_VIEW_YZ) % 4);
        if(CAMERA_VIEW_XZ <= v1 && v2 <= CAMERA_VIEW_XZ_270)
            return CameraView(CAMERA_VIEW_XZ + (1 + v1 + v2 - 2*CAMERA_VIEW_XZ) % 4);

        return is_greater ? v2 : v1;    // i.e. first original arg
    }

    bool IsXY(const CameraView v)
    {
        if(v == CAMERA_VIEW_FREE || v == CAMERA_VIEW_XYZ)
            return false;
        return CAMERA_VIEW_XY <= v && v <= CAMERA_VIEW_XY_270;
    }
    bool IsYZ(const CameraView v)
    {
        if(v == CAMERA_VIEW_FREE || v == CAMERA_VIEW_XYZ)
            return false;
        return CAMERA_VIEW_YZ <= v && v <= CAMERA_VIEW_YZ_270;
    }
    bool IsXZ(const CameraView v)
    {
        if(v == CAMERA_VIEW_FREE || v == CAMERA_VIEW_XYZ)
            return false;
        return CAMERA_VIEW_XZ <= v && v <= CAMERA_VIEW_XZ_270;
    }
}
