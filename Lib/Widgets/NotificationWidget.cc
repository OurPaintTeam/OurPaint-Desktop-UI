#include "NotificationWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPainter>


UI::NotificationWidget::NotificationWidget(const QString& text, QWidget* parent)
    : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, false);
    layout_ = new QHBoxLayout(this);
    label_ = new QLabel(text, this);
    closeButton_ = new QPushButton("✕", this);

    layout_->addWidget(label_);
    layout_->addWidget(closeButton_);
    setObjectName("NotificationWidget");
    label_->setObjectName("NotificationWidgetLabel");
    closeButton_->setObjectName("NotificationWidgetClose");
    connect(closeButton_, &QPushButton::clicked, this, &NotificationWidget::onDeleteClicked);
}


void UI::NotificationWidget::onDeleteClicked() {
    emit deleted(this);
}
