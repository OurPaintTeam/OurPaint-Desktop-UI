#include "PainterWidget.h"

UI::PainterWidget::PainterWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(QStringLiteral(R"(
        background: #ffffff;
    )"));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setObjectName(QStringLiteral("PainterWidget"));
}