#include "gui/ogl_widget.hpp"
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QDateTime>
#include <QMouseEvent>
#include "gui/renderer.hpp"
#include "defs/arch.hpp"
#include "geom/rect.hpp"


OglWidget::OglWidget(QWidget* p)
    : Base(p)
    , renderer_(new gui::Renderer())
{
    setBackgroundRole(QPalette::NoRole);
    setMouseTracking(true);
}

OglWidget::~OglWidget()
{
    SAFE_DEL(renderer_);
}

gui::Renderer* OglWidget::GetRenderer() { return renderer_; }

void OglWidget::initializeGL()
{
    Base::initializeGL();
    setAutoFillBackground(true);

    renderer_->Init((void*)winId());
}

void OglWidget::paintGL()
{
    makeCurrent();

    Base::paintGL();
    renderer_->Render();

    doneCurrent();
}

void OglWidget::resizeGL(int w, int h)
{
    Base::resizeGL(w, h);

    renderer_->OnResize(w, h);
}

void OglWidget::mousePressEvent(QMouseEvent* ev)
{
    Base::mousePressEvent(ev);

    const auto btn { (gui::MouseButtonBit)ev->button() };
    click_time_lst_[btn] = QDateTime::currentMSecsSinceEpoch();

    renderer_->OnMouseButtonDown({ btn, (int)ev->modifiers(), ev->x(), ev->y() });
}

void OglWidget::mouseReleaseEvent(QMouseEvent* ev)
{
    Base::mouseReleaseEvent(ev);

    const auto btn { (gui::MouseButtonBit)ev->button() };
    renderer_->OnMouseButtonUp({ btn, (int)ev->modifiers(), ev->x(), ev->y() },
                               (QDateTime::currentMSecsSinceEpoch() - click_time_lst_[btn]) < MOUSE_SINGLE_CLICK_MSTIME);
    click_time_lst_.remove(btn);
}

void OglWidget::mouseMoveEvent(QMouseEvent* ev)
{
    Base::mouseMoveEvent(ev);

    renderer_->OnMouseMove({ ev->x(), ev->y() }, gui::KeyboardModif((int)ev->modifiers()));
}

void OglWidget::wheelEvent(QWheelEvent* ev)
{
    Base::wheelEvent(ev);
    renderer_->OnMouseWheel({ ev->x(), ev->y() }, ev->delta());
}
