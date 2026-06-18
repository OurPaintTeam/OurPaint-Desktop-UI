#include "PainterWidget.h"

#include <QMainWindow>
#include <QOpenGLWindow>
#include <QVBoxLayout>


UI::PainterWidget::PainterWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setObjectName(QStringLiteral("PainterWidget"));


    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    stackedWidget_ = new QStackedWidget(this);
    mainLayout->addWidget(stackedWidget_);
}


void UI::PainterWidget::setQOpenGL(QOpenGLWindow* renderWindow) {}


void UI::PainterWidget::setQWindowRender(QWindow* renderWindow) {
    if (!renderWindow) return;

    auto it = containerMap_.find(renderWindow);
    if (it != containerMap_.end()) {
        stackedWidget_->setCurrentWidget(it->second);
        return;
    }

    auto* container = QWidget::createWindowContainer(renderWindow, this);
    container->setFocusPolicy(Qt::StrongFocus);
    stackedWidget_->addWidget(container);
    containerMap_[renderWindow] = container;
    stackedWidget_->setCurrentWidget(container);
}

void UI::PainterWidget::removeQWindowRender(QWindow* renderWindow) {
    auto it = containerMap_.find(renderWindow);
    if (it != containerMap_.end()) {
        stackedWidget_->removeWidget(it->second);
        it->second->deleteLater();
        containerMap_.erase(it);
    }
}
