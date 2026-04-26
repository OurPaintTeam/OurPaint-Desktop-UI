// NotificationWidget.cpp
#include "NotificationWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPainter>

namespace UI {

NotificationWidget::NotificationWidget(const QString& text, QWidget* parent)
    : QWidget(parent) {

  setAttribute(Qt::WA_StyledBackground, true);
  setAttribute(Qt::WA_TranslucentBackground, false);

  // Устанавливаем фиксированную максимальную ширину
  setMaximumWidth(300);

  layout_ = new QHBoxLayout(this);

  label_ = new QLabel(this);
  closeButton_ = new QPushButton("✕", this);
  closeButton_->setFixedSize(20, 20);

  // Настройка QLabel для переноса текста
  label_->setWordWrap(true);           // Включаем перенос слов
  label_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  label_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  // Устанавливаем текст
  label_->setText(text);

  layout_->addWidget(label_, 1);       // stretch factor = 1 (занимает всё свободное место)
  layout_->addWidget(closeButton_, 0); // stretch factor = 0 (фиксированный размер)

  setObjectName("NotificationWidget");
  label_->setObjectName("NotificationWidgetLabel");
  closeButton_->setObjectName("NotificationWidgetClose");

  connect(closeButton_, &QPushButton::clicked, this, &NotificationWidget::onDeleteClicked);
}

void NotificationWidget::onDeleteClicked() {
  emit deleted(this);
}

} // namespace UI