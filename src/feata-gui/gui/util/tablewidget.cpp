#include "gui/util/tablewidget.hpp"
#include <QMouseEvent>


namespace gui
{
    TableWidget::TableWidget(QWidget* parent)
        : Base(parent)
    {}

    TableWidget::TableWidget(int row, int col, QWidget* parent)
        : Base(row, col, parent)
    {}

    void TableWidget::mousePressEvent(QMouseEvent* ev)
    {
        if(selectionMode() == Base::SingleSelection)
        {
            const auto item = itemAt(ev->pos());

            if(!item || item->isSelected())
            {
                clearSelection();
                return;
            }
        }

        return Base::mousePressEvent(ev);
    }
}
