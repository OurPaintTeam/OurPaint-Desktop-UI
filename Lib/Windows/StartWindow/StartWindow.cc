#include "StartWindow.h"

#include "StartPage.h"

#include <QVBoxLayout>

namespace {
    constexpr int MARGIN = 0;
    constexpr int SPACING = 0;
} // namespace


UI::StartWindow::StartWindow(QWidget* parent)
    : BaseWindow(parent) {
    initUI();
}


void UI::StartWindow::initUI() {
    contentWidget_ = new QWidget(this);
    setCentralWidget(contentWidget_);

    rootLayout_ = new QVBoxLayout(contentWidget_);
    rootLayout_->setContentsMargins(MARGIN, MARGIN, MARGIN, MARGIN);
    rootLayout_->setSpacing(SPACING);

    startPage_ = new StartPage(contentWidget_);
    rootLayout_->addWidget(startPage_);

    connect(startPage_, &StartPage::openProjectInThisWindowTriggered,
            this, &StartWindow::openProjectThisWindowTriggered);
    connect(startPage_, &StartPage::createProjectInThisWindowTriggered,
            this, &StartWindow::createProjectThisWindowTriggered);
    connect(startPage_, &StartPage::renameProjectTriggered,
            this, &StartWindow::renameProjectTriggered);
    connect(startPage_, &StartPage::deleteProjectTriggered,
            this, &StartWindow::deleteProjectTriggered);

    initNotificationManager();
}


void UI::StartWindow::setProjectsList(const QVector<QPair<QString, QString>>& projectsList) const {
    if (startPage_) {
        startPage_->setProjectsList(projectsList);
    }
}


void UI::StartWindow::setDefaultProjectsPath(const QString& projectPath) const {
    if (startPage_) {
        startPage_->setDefaultProjectsPath(projectPath);
    }
}
