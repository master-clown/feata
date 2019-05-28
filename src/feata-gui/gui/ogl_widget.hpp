#pragma once

#include <QOpenGLWidget>
#include "stl/map.hpp"
#include "stl/string.hpp"
#include "gui/select_defs.hpp"
#include "gui/input_defs.hpp"


namespace gui { class Renderer; }

class OglWidget
        : public QOpenGLWidget
{
public:
    OglWidget(QWidget* p = nullptr);
    ~OglWidget() override;

    gui::Renderer* GetRenderer();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    //void paintEvent(QPaintEvent* ev) override;

    void mousePressEvent(QMouseEvent* ev) override;
    void mouseReleaseEvent(QMouseEvent* ev) override;
    void mouseMoveEvent(QMouseEvent* ev) override;
    void wheelEvent(QWheelEvent* ev) override;

private:
    gui::Renderer* renderer_;
    Map<gui::MouseButtonBit, quint64> click_time_lst_;

    using Base = QOpenGLWidget;
};
