#include "NotificationWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>


UI::NotificationWidget::NotificationWidget(const QString& text, QWidget* parent)
    : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    layout_ = new QHBoxLayout(this);
    label_ = new QLabel(text, this);
    closeButton_ = new QPushButton("✕", this);

    layout_->addWidget(label_);
    layout_->addWidget(closeButton_);
    setObjectName("NotificationWidget");
    connect(closeButton_, &QPushButton::clicked, this, &NotificationWidget::onDeleteClicked);
}


void UI::NotificationWidget::onDeleteClicked() {
    emit deleted(this);
}
