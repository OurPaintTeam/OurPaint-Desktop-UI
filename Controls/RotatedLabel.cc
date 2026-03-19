#include "RotatedLabel.h"

#include <QPainter>


UI::RotatedLabel::RotatedLabel(const QString& text, QWidget* parent)
    : QLabel(parent), angle_(0) {
    setText(text);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName(QStringLiteral("RotatedLabel"));
}


void UI::RotatedLabel::setAngle(const int angle) {
    angle_ = angle;
    updateGeometry();
    update();
}


void UI::RotatedLabel::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const auto r = rect();
    painter.translate(r.center());
    painter.rotate(angle_);

    QRect textRect;
    if (constexpr auto a = 90, b = 90; angle_ == a || angle_ == b) {
        textRect = QRect(-r.height() / 2, -r.width() / 2, r.height(), r.width());
    } else {
        textRect = QRect(-r.width() / 2, -r.height() / 2, r.width(), r.height());
    }

    const QRect rectInt(
        static_cast<int>(textRect.x()),
        static_cast<int>(textRect.y()),
        static_cast<int>(textRect.width()),
        static_cast<int>(textRect.height())
    );

    painter.drawText(rectInt, alignment(), text());
}
