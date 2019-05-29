QT += gui widgets opengl

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

win32
{
    RC_ICONS = ../../resource/win/feata.ico
}

FORMS += \
    $$PWD/gui/mainwindow.ui

HEADERS += \
    $$PWD/gui/mainwindow.hpp \
    $$PWD/gui/renderer.hpp \
    $$PWD/gui/ogl_widget.hpp \
    $$PWD/gui/inputmgr.hpp \
    $$PWD/gui/select_defs.hpp \
    $$PWD/gui/input_defs.hpp \
    $$PWD/gui/toolbar_cmd.hpp \
    $$PWD/gui/wnd/logwidget.hpp \
    $$PWD/gui/wnd/project_tree.hpp \
    $$PWD/gui/renderlist.hpp \
    $$PWD/gui/renderobjtype.hpp \
    $$PWD/gui/wgt/widget_bool.hpp \
    $$PWD/gui/wgt/widget_int.hpp \
    $$PWD/gui/wgt/basewidget.hpp \
    $$PWD/gui/wgt/widget_real.hpp \
    $$PWD/gui/wgt/widget_string.hpp \
    $$PWD/gui/wgt/widget_strchoice.hpp \
    $$PWD/gui/wgt/widget_geomsel.hpp \
    $$PWD/gui/util/tablewidget.hpp \
    $$PWD/gui/wgt/basewidgetgeomsel.hpp \
    $$PWD/gui/wgt/widgetmgr.hpp \
    $$PWD/gui/wgt/widget_triang.hpp \
    $$PWD/gui/dlg/filedlg.hpp \
    $$PWD/gui/dlg/mboxdlg.hpp \
    $$PWD/gui/util/treewidgetitem.hpp \
    $$PWD/gui/util/treewidgetitem_type.hpp \
    $$PWD/gui/wgt/widget_label.hpp \
    $$PWD/gui/wgt/widgetsetscont.hpp \
    $$PWD/gui/wgt/widget_geomselstr.hpp \
    $$PWD/gui/util/occt_intcontext.hpp

SOURCES += \
    $$PWD/gui/mainwindow.cpp \
    $$PWD/gui/renderer.cpp \
    $$PWD/gui/ogl_widget.cpp \
    $$PWD/gui/inputmgr.cpp \
    $$PWD/gui/toolbar_cmd.cpp \
    $$PWD/gui/wnd/logwidget.cpp \
    $$PWD/gui/renderlist.cpp \
    $$PWD/gui/wgt/widget_bool.cpp \
    $$PWD/gui/wgt/widget_int.cpp \
    $$PWD/gui/wgt/basewidget.cpp \
    $$PWD/gui/wgt/widget_real.cpp \
    $$PWD/gui/wgt/widget_string.cpp \
    $$PWD/gui/wgt/widget_strchoice.cpp \
    $$PWD/gui/wgt/widget_geomsel.cpp \
    $$PWD/gui/util/tablewidget.cpp \
    $$PWD/gui/wgt/widgetmgr.cpp \
    $$PWD/gui/wgt/widget_triang.cpp \
    $$PWD/gui/dlg/filedlg.cpp \
    $$PWD/gui/dlg/mboxdlg.cpp \
    $$PWD/gui/util/treewidgetitem.cpp \
    $$PWD/gui/wgt/widget_label.cpp \
    $$PWD/gui/wgt/widgetsetscont.cpp \
    $$PWD/gui/wgt/widget_geomselstr.cpp \
    $$PWD/gui/util/occt_intcontext.cpp

RESOURCES += \
    $$PWD/../../resource/icons.qrc

DISTFILES += \
    $$PWD/gui/wgt/widget_geomselrealmapcpp

