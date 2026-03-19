#include "CustomMenu.h"

#include <QGraphicsDropShadowEffect>


void UI::CustomMenu::showEvent(QShowEvent* event) {
    QMenu::showEvent(event);
    if (graphicsEffect()) {
        delete graphicsEffect();
        setGraphicsEffect(nullptr);
    }
}


UI::CustomMenu::CustomMenu(QWidget* parent)
    : QMenu(parent) {
    setupWindow();
    setObjectName(QStringLiteral("CustomMenu"));
}


void UI::CustomMenu::setupWindow() {
    // Remove window frame
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

    // Enable transparent background
    setAttribute(Qt::WA_TranslucentBackground, true);
}
