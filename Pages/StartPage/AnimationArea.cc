#include "AnimationArea.h"
#include "AnimationWidget.h"


UI::AnimationArea::AnimationArea(QWidget* parent)
    : QWidget(parent),
      layout_(new QHBoxLayout(this)),
      animationWidget_(new AnimationWidget(this)) {
    layout_->setContentsMargins(0, 0, 0, 0);

    setObjectName("AnimationArea");
    setAttribute(Qt::WA_StyledBackground, true);

    layout_->addWidget(animationWidget_);
}
