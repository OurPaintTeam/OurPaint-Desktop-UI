#include "TopBarStart.h"

#include <QEvent>
#include <QLabel>


UI::TopBarStart::TopBarStart(QWidget *parent)
    : TopBar(parent) {
    title_ = new QLabel("", this);
    title_->setObjectName(QStringLiteral("TitleOurPaint"));
    addLeftWidget(title_);
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName("TopBarStart");
    translate();
}


void UI::TopBarStart::changeEvent(QEvent *e) {
    if (e->type() == QEvent::LanguageChange) {
        translate();
    }

    TopBar::changeEvent(e);
}


void UI::TopBarStart::translate() const {
    if (title_) {
    title_->setText(UI::TopBarStart::tr("OurPaint"));
    }
}
