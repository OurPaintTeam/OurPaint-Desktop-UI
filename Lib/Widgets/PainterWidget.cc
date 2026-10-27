#include "PainterWidget.h"

#include <QMainWindow>
#include <QOpenGLWindow>
#include <QVBoxLayout>


UI::PainterWidget::PainterWidget(QWidget* parent) : QWidget(parent), layout_(new QVBoxLayout(this)) {
    setAttribute(Qt::WA_StyledBackground, true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setObjectName(QStringLiteral("PainterWidget"));
    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(0);
    setLayout(layout_);
}


void UI::PainterWidget::setQOpenGL(QOpenGLWindow* renderWindow) {
    // OpenGL setup
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    constexpr auto depth = 24;
    constexpr auto stencil = 8;
    format.setDepthBufferSize(depth);
    format.setStencilBufferSize(stencil);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);

    renderWindow->setFormat(format);

    auto* renderWidget = QWidget::createWindowContainer(renderWindow, this);
    renderWidget->setFocusPolicy(Qt::StrongFocus);
    layout_->addWidget(renderWidget);
}


void UI::PainterWidget::setQWindowRender(QWindow* renderWindow) {
    auto* renderWidget = QWidget::createWindowContainer(renderWindow, this);
    renderWidget->setFocusPolicy(Qt::StrongFocus);
    layout_->addWidget(renderWidget);
}