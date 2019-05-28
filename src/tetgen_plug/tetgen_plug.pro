#-------------------------------------------------
#
# Project created by QtCreator 2019-04-27T19:18:03
#
#-------------------------------------------------

TARGET = tetgen_plug
TEMPLATE = lib
CONFIG += c++17

DEFINES += TETGEN_PLUG_LIBRARY TETLIBRARY

json_copy.path = $$PWD/../../test/plugins
json_copy.files += $$files($$PWD/*.json)
dll_copy.path = $$PWD/../../test/plugins

CONFIG(debug, debug|release) {
    dll_copy.files += $$files($$OUT_PWD/debug/*.dll)
    dll_copy.files += $$files($$OUT_PWD/debug/*.pdb)
}
else {
    dll_copy.files += $$files($$OUT_PWD/release/*.dll)  $$files($$OUT_PWD/debug/*.pdb)
}


INSTALLS += dll_copy
INSTALLS += json_copy


#win32: LIBS += -L$$PWD/../../3rdparty/win/tetgen-1.5.1/lib/ -ltetgen
#INCLUDEPATH += $$PWD/../../3rdparty/win/tetgen-1.5.1/include
#DEPENDPATH += $$PWD/../../3rdparty/win/tetgen-1.5.1/include

INCLUDEPATH += $$PWD/../shared

SOURCES += \
        tetgenmeshplug.cpp \
    predicates.cxx \
    tetgen.cxx

HEADERS += \
        tetgenmeshplug.hpp \
    tetgen.h
