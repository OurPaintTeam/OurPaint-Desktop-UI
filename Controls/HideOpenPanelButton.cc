#include "HideOpenPanelButton.h"

#include <QPainter>

namespace {
    constexpr int K_ARROW_SIZE_PX = 5;
    constexpr int K_ARROW_PEN_WIDTH_PX = 2;
} // namespace

UI::HideOpenPanelButton::HideOpenPanelButton(QWidget* parent)
    : QPushButton(parent) {
    setCheckable(true);
    setFocusPolicy(Qt::NoFocus);
    setStyleSheet(R"(
        QPushButton {
            background: transparent;
            color: white;
            border: none;
            border-radius: 6px;
        }
        QPushButton:hover {
            background: rgba(255,255,255,0.15);
        }
    )");
    setObjectName(QStringLiteral("HideOpenPanelButton"));
}


void UI::HideOpenPanelButton::setVertical(const bool v) {
    vertical_ = v;
    update();
}


bool UI::HideOpenPanelButton::isVertical() const {
    return vertical_;
}


void UI::HideOpenPanelButton::paintEvent(QPaintEvent* event) {
    QPushButton::paintEvent(event);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QColor color(QStringLiteral("#D8D8F6"));
    p.setPen(QPen(color, K_ARROW_PEN_WIDTH_PX, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    // Button center.
    const QPoint center(width() / 2, height() / 2);

    constexpr QPoint left(-K_ARROW_SIZE_PX, -K_ARROW_SIZE_PX / 4);
    constexpr QPoint bottom(0, K_ARROW_SIZE_PX / 3);
    constexpr QPoint right(K_ARROW_SIZE_PX, -K_ARROW_SIZE_PX / 4);

    QTransform transform;
    transform.translate(center.x(), center.y());

    if (vertical_) {
        constexpr auto angle = 90;
        transform.rotate(angle);
    }

    if (isChecked()) {
        constexpr auto angle = 180;
        transform.rotate(angle);
    }

    const QPointF leftT = transform.map(left);
    const QPointF bottomT = transform.map(bottom);
    const QPointF rightT = transform.map(right);

    p.drawLine(leftT, bottomT);
    p.drawLine(bottomT, rightT);
}
