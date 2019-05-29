QT += core gui widgets
TARGET = FEATA
TEMPLATE = app

CONFIG += c++17
#CONFIG += force_debug_info

DEFINES += FEATA_APP


# 3RD PARTY

unix:!macx {
}

win32 {
    DEPENDPATH += $$PWD/../../3rdparty/win/opencascade-7.3.0/include
    INCLUDEPATH += $$PWD/../../3rdparty/win/opencascade-7.3.0/include
    LIBS += -L$$PWD/../../3rdparty/win/opencascade-7.3.0/lib/ -lTKMath -lTKBRep -lTKTopAlgo -lTKGeomBase  \
                                                              -lTKG2d -lTKG3d -lTKOffset -lTKPrim -lTKFillet -lTKBO \
                                                              -lTKOpenGl -lTKService -lTKSTEP -lTKXSBase    \
                                                              -lTKMesh -lTKMeshVS -lTKSTL -lTKV3d -lTKernel \

}


# Project

include(../shared/shared.pri)
include(../feata-gui/feata-gui.pri)

SOURCES += \
        main.cpp \
    util/console.cpp \
    util/datetime.cpp \
    util/log.cpp \
    util/thread.cpp \
    util/filesys.cpp \
    util/cmdarglist.cpp \
    test/test.cpp \
    data/textstream.cpp \
    data/binarystream.cpp \
    plug/version.cpp \
    plug/services.cpp \
    plug/pluginmgr.cpp \
    util/shared_lib.cpp \
    core/shlib_mgr.cpp \
    data/json.cpp \
    plug/plugin.cpp \
    defs/error.cpp \
    test/occ_test.cpp \
    proj/glob_events.cpp \
    geom/mdl/stepmodel.cpp \
    proj/project.cpp \
    geom/mdl/convert.cpp \
    geom/mdl/triangulate.cpp \
    mesh/meshdatasource.cpp \
    mesh/meshvis.cpp \
    geom/featageom.cpp \
    plug/pluginsets.cpp \
    geom/mdl/mdl_type.cpp \
    geom/mdl/stlmodel.cpp \
    mesh/featamesh.cpp \
    geom/mdl/shapetriang.cpp \
    geom/geomvis.cpp \
    mesh/mesher.cpp \
    plug/pluginsets_types.cpp \
    geom/geom_ent_type.cpp \
    core/comp_def.cpp \
    proj/glob_obj.cpp \
    plug/plugininfo.cpp \
    solv/solver.cpp \
    solv/featasolv.cpp \
    post/nsolmeshvis.cpp \
    post/featapost.cpp

HEADERS += \
    util/console.hpp \
    util/datetime.hpp \
    util/filesys.hpp \
    util/log.hpp \
    util/thread.hpp \
    util/cmdarglist.hpp \
    test/test.hpp \
    data/textstream.hpp \
    data/binarystream.hpp \
    data/basestream.hpp \
    geom/point.hpp \
    plug/version.hpp \
    plug/pluginmgr.hpp \
    plug/services.hpp \
    util/shared_lib.hpp \
    core/shlib_mgr.hpp \
    data/json.hpp \
    plug/plugin.hpp \
    defs/arch.hpp \
    defs/common.hpp \
    defs/maths.hpp \
    defs/matrix.hpp \
    defs/vec.hpp \
    defs/error.hpp \
    geom/rect.hpp \
    util/event.hpp \
    proj/glob_events.hpp \
    geom/geom_ent_type.hpp \
    geom/mdl/basemodel.hpp \
    geom/mdl/stepmodel.hpp \
    proj/project.hpp \
    geom/mdl/convert.hpp \
    geom/mdl/triangulate.hpp \
    mesh/meshdatasource.hpp \
    mesh/meshvis.hpp \
    geom/featageom.hpp \
    mesh/featamesh.hpp \
    stl/vector.hpp \
    stl/list.hpp \
    stl/map.hpp \
    stl/hashtable.hpp \
    stl/array.hpp \
    stl/queue.hpp \
    stl/set.hpp \
    stl/bytearray.hpp \
    mesh/mesher.hpp \
    plug/plugid_t.hpp \
    geom/mdl/mdl_type.hpp \
    plug/pluginsets.hpp \
    geom/mdl/stlmodel.hpp \
    geom/mdl/shapetriang.hpp \
    geom/geomvis.hpp \
    plug/pluginsets_types.hpp \
    geom/gid_t.hpp \
    util/event_id.hpp \
    core/comp_def.hpp \
    core/comp_pluggable.hpp \
    proj/glob_obj.hpp \
    util/paralleltask.hpp \
    plug/plugininfo.hpp \
    util/dirty_int.hpp \
    solv/featasolv.hpp \
    solv/solver.hpp \
    post/featapost.hpp \
    post/nodalsol.hpp \
    post/nsolmeshvis.hpp \
    post/datasetinfo.hpp
