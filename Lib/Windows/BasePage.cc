#include "BasePage.h"

#include <QVBoxLayout>

namespace {
    constexpr int LAYOUT_MARGIN = 0;
    constexpr int LAYOUT_SPACING = 0;
} // namespace UI


UI::BasePage::BasePage(QWidget* parent)
    : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
}


void UI::BasePage::initBasePage() {
    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->setContentsMargins(LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN);
    mainLayout_->setSpacing(LAYOUT_SPACING);

    mainArea_ = new QWidget(this);
    mainLayout_->addWidget(mainArea_, 1);
}
