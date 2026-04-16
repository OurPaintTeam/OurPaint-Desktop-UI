#include "TopBarStart.h"

#include <QLabel>


UI::TopBarStart::TopBarStart(QWidget* parent)
    : TopBar(parent) {
    title_ = new QLabel("OurPaint", this);
    title_->setObjectName(QStringLiteral("TitleOurPaint"));
    addLeftWidget(title_);
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName("TopBarStart");
}
