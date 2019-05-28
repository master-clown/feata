#pragma once


namespace gui::cmd
{
    enum CameraView
    {
        CAMERA_VIEW_FREE,
        CAMERA_VIEW_XY,
        CAMERA_VIEW_XY_90,
        CAMERA_VIEW_XY_180,
        CAMERA_VIEW_XY_270,
        CAMERA_VIEW_YZ,
        CAMERA_VIEW_YZ_90,
        CAMERA_VIEW_YZ_180,
        CAMERA_VIEW_YZ_270,
        CAMERA_VIEW_XZ,
        CAMERA_VIEW_XZ_90,
        CAMERA_VIEW_XZ_180,
        CAMERA_VIEW_XZ_270,
        CAMERA_VIEW_XYZ      // standard 2-simplex
    };

    CameraView ComposeCameraView(CameraView v1, CameraView v2);
    bool IsXY(const CameraView v);
    bool IsYZ(const CameraView v);
    bool IsXZ(const CameraView v);

    enum ViewMode
    {
        VIEW_MODE_SOLID,
        VIEW_MODE_WIREFRAME
    };
}
