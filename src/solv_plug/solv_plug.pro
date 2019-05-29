#-------------------------------------------------
#
# Project created by QtCreator 2019-04-27T19:18:03
#
#-------------------------------------------------

TARGET = solv_plug
TEMPLATE = lib
CONFIG += c++17


CONFIG(debug, debug|release) {
    json_copy.path = $$PWD/../../test/plugins_dbg
    dll_copy.path = $$PWD/../../test/plugins_dbg

    dll_copy.files += $$files($$OUT_PWD/debug/*.dll)
    dll_copy.files += $$files($$OUT_PWD/debug/*.pdb)
}
else {
    json_copy.path = $$PWD/../../test/plugins
    dll_copy.path = $$PWD/../../test/plugins
    dll_copy.files += $$files($$OUT_PWD/release/*.dll)
    dll_copy.files += $$files($$OUT_PWD/release/*.pdb)
}

json_copy.files += $$files($$PWD/*.json)

INSTALLS += dll_copy
INSTALLS += json_copy


INCLUDEPATH += $$PWD/../../3rdparty/cross/eigen-3.3.7/include

INCLUDEPATH += $$PWD/../shared

SOURCES += \
        solv_plug.cpp \
    solver.cpp \
    tetelem.cpp \
    solver_datasets.cpp

HEADERS += \
        solv_plug.hpp \
    tetelem.hpp \
    mathdef.hpp
