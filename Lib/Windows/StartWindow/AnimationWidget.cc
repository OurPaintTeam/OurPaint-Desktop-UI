#include "AnimationWidget.h"

#include <QLabel>
#include <QPainter>
#include <QResizeEvent>
#include <QVBoxLayout>


UI::AnimationWidget::AnimationWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);

    label_ = new QLabel(this);
    label_->setAlignment(Qt::AlignCenter);
    label_->setWordWrap(true);
    label_->setObjectName("AreaLabel");

    auto* l = new QVBoxLayout(this);
    l->setContentsMargins(0, 0, 0, 0);
    l->addWidget(label_, 0, Qt::AlignCenter);

    translate();
}


void UI::AnimationWidget::resizeEvent(QResizeEvent* event)
{
    if(label_)
    {
        const int x = (width() - label_->width()) / 2;
        const int y = (height() - label_->height()) / 2;

        label_->move(x,y);
    }

    QWidget::resizeEvent(event);
}


void UI::AnimationWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    constexpr QColor backgroundColor(73,72,80);

    painter.fillRect(rect(), backgroundColor);
}

void UI::AnimationWidget::changeEvent(QEvent* e) {
    if (e && e->type() == QEvent::LanguageChange) {
        translate();
    }
    QWidget::changeEvent(e);
}

void UI::AnimationWidget::translate() const {
    if (!label_) return;

    label_->setText(UI::AnimationWidget::tr("In development..."));

    label_->adjustSize();

    const int x = (width()  - label_->width())  / 2;
    const int y = (height() - label_->height()) / 2;
    label_->move(x, y);
}