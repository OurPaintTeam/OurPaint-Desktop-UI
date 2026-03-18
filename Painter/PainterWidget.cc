#include "PainterWidget.h"

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
    auto* renderWidget = QWidget::createWindowContainer(renderWindow, this);
    renderWidget->setFocusPolicy(Qt::StrongFocus);
    layout_->addWidget(renderWidget);
}
