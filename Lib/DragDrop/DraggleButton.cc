#include "DraggleButton.h"

#include <QMouseEvent>

namespace {
    constexpr int K_BUTTON_SIZE_PX = 25;
} // namespace


UI::DraggleButton::DraggleButton(const QString& text,
                                 QWidget* parent)
    : QPushButton(text, parent) {
    setFixedSize(K_BUTTON_SIZE_PX, K_BUTTON_SIZE_PX);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
    setObjectName(QStringLiteral("DoubleClickButton"));
}


UI::DraggleButton::DraggleButton(const QIcon& icon,
                                 QWidget* parent)
    : QPushButton(parent) {
    setIcon(icon);
    setFixedSize(K_BUTTON_SIZE_PX, K_BUTTON_SIZE_PX);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
    setObjectName(QStringLiteral("DoubleClickButton"));
}


void UI::DraggleButton::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::RightButton) {
        emit rightClicked();
    }

    QPushButton::mousePressEvent(e);
}
