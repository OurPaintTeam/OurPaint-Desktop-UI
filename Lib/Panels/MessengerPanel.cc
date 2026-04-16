#include "MessengerPanel.h"

#include <QHBoxLayout>
#include <QPushButton>


UI::MessengerPanel::MessengerPanel(QWidget* parent)
    : QWidget(parent),
      layout_(new QHBoxLayout(this)) {
    layout_->setContentsMargins(0, 0, 0, 0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_StyledBackground, true);

    createButtonBack();
    setObjectName(QStringLiteral("MessengerPanel"));
}


void UI::MessengerPanel::createButtonBack() {
    btnBack_ = new QPushButton("←", this);
    constexpr auto buttonSize = QSize(25, 25);
    btnBack_->setFixedSize(buttonSize);

    layout_->addStretch();
    layout_->addWidget(btnBack_);
}


QPushButton* UI::MessengerPanel::getButtonBack() const {
    return btnBack_;
}
