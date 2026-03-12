#include "CentralWidget.h"


UI::CentralWidget::CentralWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(QStringLiteral("background-color: #978897;"));
    setObjectName(QStringLiteral("CentralWidget"));
}
