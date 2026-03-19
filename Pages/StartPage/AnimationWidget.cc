#include "AnimationWidget.h"

#include <QPainter>
#include <QResizeEvent>

UI::AnimationWidget::AnimationWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);

    label_ = new QLabel("In development...", this);

    auto font = label_->font();
    constexpr auto x = 24;
    font.setPointSize(x);
    font.setBold(true);

    label_->setFont(font);
    label_->adjustSize();
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