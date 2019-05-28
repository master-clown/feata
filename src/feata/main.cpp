#include <QApplication>
#include "util/cmdarglist.hpp"
#include "gui/mainwindow.hpp"


int main(int argc, char *argv[])
{
    util::CmdArgList arg_lst(argc, argv);

    QApplication app(argc, argv);
    gui::MainWindow mw;

    mw.show();

    return app.exec();
}
