#include "SettingsWidget.h"

#include <QPushButton>
#include <QVBoxLayout>


UI::SettingsWidget::SettingsWidget(QWidget* parent)
    : QWidget(parent), layout_(new QVBoxLayout(this)) {
    constexpr auto size = QSize(360, 220);
    setFixedSize(size);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_StyledBackground, true);

    constexpr auto sizeClB = QSize(28, 28);
    constexpr auto margins = 8;
    layout_->setContentsMargins(margins, margins, margins, margins);
    closeButton_ = new QPushButton("✕", this);
    closeButton_->setFixedSize(sizeClB);
    closeButton_->setFocusPolicy(Qt::NoFocus);
    closeButton_->setCursor(Qt::PointingHandCursor);

    connect(closeButton_, &QPushButton::clicked, this, &QWidget::close);

    layout_->addWidget(closeButton_, 0, Qt::AlignRight | Qt::AlignTop);
    layout_->addStretch();

    setObjectName(QStringLiteral("SettingsWidget"));
}
