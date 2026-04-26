#include "AnimationWidget.h"

#include <QLabel>
#include <QPainter>
#include <QResizeEvent>

UI::AnimationWidget::AnimationWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent);

    label_ = new QLabel("", this);

    auto font = label_->font();
    constexpr auto x = 24;
    font.setPointSize(x);
    font.setBold(true);

    label_->setFont(font);
    label_->adjustSize();
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
    if (label_) {
        label_->setText(UI::AnimationWidget::tr("In development..."));
    }
}