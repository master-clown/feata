#-------------------------------------------------
#
# Project created by QtCreator 2019-04-27T19:18:03
#
#-------------------------------------------------

TARGET = solv_plug
TEMPLATE = lib
CONFIG += c++17


json_copy.path = $$PWD/../../test/plugins
json_copy.files += $$files($$PWD/*.json)
dll_copy.path = $$PWD/../../test/plugins

CONFIG(debug, debug|release) {
    dll_copy.files += $$files($$OUT_PWD/debug/*.dll)
    dll_copy.files += $$files($$OUT_PWD/debug/*.pdb)
}
else {
    dll_copy.files += $$files($$OUT_PWD/release/*.dll)
    dll_copy.files += $$files($$OUT_PWD/debug/*.pdb)
}

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
