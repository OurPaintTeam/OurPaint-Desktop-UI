#include "InformationPanel.h"

#include <QHBoxLayout>
#include <QPushButton>


UI::InformationPanel::InformationPanel(QWidget* parent)
    : QWidget(parent),
      layout_(new QHBoxLayout(this)) {
    layout_->setContentsMargins(0, 0, 0, 0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAttribute(Qt::WA_StyledBackground, true);

    createButtonBack();
    setObjectName(QStringLiteral("InformationPanel"));
}


void UI::InformationPanel::createButtonBack() {
    btnBack_ = new QPushButton("←", this);
    constexpr auto buttonSize = QSize(25, 25);
    btnBack_->setFixedSize(buttonSize);
    layout_->addStretch();
    layout_->addWidget(btnBack_);
}


QPushButton* UI::InformationPanel::getButtonBack() const {
    return btnBack_;
}
