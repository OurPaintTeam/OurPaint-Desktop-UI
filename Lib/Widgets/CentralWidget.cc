#include "CentralWidget.h"


UI::CentralWidget::CentralWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName(QStringLiteral("CentralWidget"));
}
