#include "gui/wgt/basewidget.hpp"
#include <QLayout>


namespace gui::wgt
{
    BaseWidget::BaseWidget(QWidget* parent)
        : Base(parent)
    {
        setFrameShape(QFrame::Shape::Box);
        setFrameShadow(QFrame::Shadow::Plain);
    }

    void BaseWidget::DestroyLayout(QLayout* lout)
    {
        if(!lout) return;

        while (auto child = lout->takeAt(0))
        {
            if(auto lch = child->layout())
                BaseWidget::DestroyLayout(lch);
            else
            if(auto wgt = child->widget())
                delete wgt;
            delete child;
        }
        delete lout;
    }
}
