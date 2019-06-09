#-------------------------------------------------
#
# Project created by QtCreator 2019-04-27T19:18:03
#
#-------------------------------------------------

TARGET = tetgen_plug
TEMPLATE = lib
CONFIG += c++17

# 3RD-PARTY

## Tetgen 1.5.1
DEFINES += TETGEN_PLUG_LIBRARY TETLIBRARY
include($$PWD/../3rdparty/tetgen-1.5.1/tetgen.pri)


# to replace standard printf to ours, so that log of tetgen was redirected
DEFINES += _NO_CRT_STDIO_INLINE
LIBS += -llegacy_stdio_definitions
QMAKE_CFLAGS += -Dprintf=printf_modified
QMAKE_CXXFLAGS += -Dprintf=printf_modified


# Install routines (to speed up testing)

#CONFIG(debug, debug|release) {
#    json_copy.path = $$PWD/../../test/plugins_dbg
#    dll_copy.path = $$PWD/../../test/plugins_dbg
#    dll_copy.files += $$files($$OUT_PWD/debug/*.dll)
#    dll_copy.files += $$files($$OUT_PWD/debug/*.pdb)
#}
#else {
#    json_copy.path = $$PWD/../../test/plugins
#    dll_copy.path = $$PWD/../../test/plugins
#    dll_copy.files += $$files($$OUT_PWD/release/*.dll)
#    dll_copy.files += $$files($$OUT_PWD/release/*.pdb)
#}

#json_copy.files += $$files($$PWD/*.json)

#INSTALLS += dll_copy
#INSTALLS += json_copy

INCLUDEPATH += $$PWD/../shared

SOURCES += \
        tetgenmeshplug.cpp \

HEADERS += \
        tetgenmeshplug.hpp \
