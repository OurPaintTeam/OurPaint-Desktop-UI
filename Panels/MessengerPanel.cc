#include "MessengerPanel.h"


UI::MessengerPanel::MessengerPanel(QWidget* parent)
    : QWidget(parent) {
    layout_ = new QHBoxLayout(this);
    layout_->setContentsMargins(0, 0, 0, 0);

    setAttribute(Qt::WA_StyledBackground, true);

    setStyleSheet(QStringLiteral(R"(
        background: #494850;
        border: none;
        border-right: 1px solid #3e3d42;
    )"));
    createButtonBack();
    setObjectName(QStringLiteral("MessengerPanel"));
}


void UI::MessengerPanel::createButtonBack() {
    btnBack_ = new QPushButton("←", this);
    constexpr auto buttonSize = QSize(25, 25);
    btnBack_->setFixedSize(buttonSize);
    btnBack_->setStyleSheet(QStringLiteral(R"(
            QPushButton {
                background: transparent;
                border: none;
                color: white;
                font-weight: bold;
            }
            QPushButton:hover {
                background: rgba(255,255,255,0.2);
            }
            QPushButton:pressed {
                background: rgba(255,255,255,0.4);
            }
        )"));


    layout_->addStretch();
    layout_->addWidget(btnBack_);
}


QPushButton* UI::MessengerPanel::getButtonBack() const {
    return btnBack_;
}
