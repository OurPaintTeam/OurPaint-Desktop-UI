#include "SettingsWidget.h"

#include <QPushButton>
#include <QVBoxLayout>


UI::SettingsWidget::SettingsWidget(QWidget* parent)
    : QWidget(parent) {
    constexpr auto size = QSize(360, 220);
    setFixedSize(size);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_StyledBackground, true);

    setStyleSheet(R"(
        SettingsWidget {
            background: #3f3e46;
            border-radius: 10px;
        }
        QPushButton {
            background: transparent;
            color: #D8D8F6;
            border: none;
            font-size: 16px;
        }
        QPushButton:hover {
            background: rgba(255,255,255,0.12);
            border-radius: 6px;
        }
    )");

    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(8, 8, 8, 8);
    closeButton_ = new QPushButton("✕", this);
    closeButton_->setFixedSize(28, 28);
    closeButton_->setFocusPolicy(Qt::NoFocus);
    closeButton_->setCursor(Qt::PointingHandCursor);

    connect(closeButton_, &QPushButton::clicked, this, &QWidget::close);

    layout_->addWidget(closeButton_, 0, Qt::AlignRight | Qt::AlignTop);
    layout_->addStretch();

    setObjectName(QStringLiteral("closeButton"));
}
