#include "TopBarStart.h"

#include <QLabel>


UI::TopBarStart::TopBarStart(QWidget* parent)
    : TopBar(parent) {
    title_ = new QLabel("OurPaint", this);
    title_->setStyleSheet("color: #D8D8F6; font-weight: bold; font-size: 16px;");
    auto* layout = getLayout();
    layout->insertWidget(0, title_);
}
