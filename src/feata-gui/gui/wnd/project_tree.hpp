#pragma once

#include <QDockWidget>


class QTreeWidget;

namespace gui::wnd
{
    class ProjectTree
            : public QDockWidget
    {
    public:
        ProjectTree(QWidget* parent = nullptr);
        ~ProjectTree() override;

    private:
        QTreeWidget* tree_;

        using Base = QDockWidget;
    };
}
