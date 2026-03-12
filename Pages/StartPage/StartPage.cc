#include "StartPage.h"

#include "AnimationArea.h"
#include "ProjectsListWidget.h"
#include "TopBarStart.h"


UI::StartPage::StartPage(QWidget* parent)
    : QWidget(parent) {
    initUI();
}


void UI::StartPage::initUI() {
    mainLayout_ = new QVBoxLayout(this);
    mainArea_ = new QWidget(this);
    contentLayout_ = new QHBoxLayout(mainArea_);

    projectsListWidget_ = new ProjectsListWidget(this);
    animationArea_ = new AnimationArea(this);

    topBar_ = new TopBarStart(this);

    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);

    // Top bar.
    mainLayout_->addWidget(topBar_);

    // Main content area.
    mainLayout_->addWidget(mainArea_);

    contentLayout_->setContentsMargins(0, 0, 0, 0);
    contentLayout_->setSpacing(0);

    contentLayout_->addWidget(projectsListWidget_);
    contentLayout_->addWidget(animationArea_);
}
