#pragma once

#include <QTableWidget>


namespace gui
{
    // Allows to deselect an item when clicked at it or empty space
    // (for single selection mode only)
    class TableWidget
            : public QTableWidget
    {
    public:
        TableWidget(QWidget* parent = nullptr);
        TableWidget(int row, int col, QWidget* parent = nullptr);

        void mousePressEvent(QMouseEvent* ev) override;

    private:
        using Base = QTableWidget;
    };
}
