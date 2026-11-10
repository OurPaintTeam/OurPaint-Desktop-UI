#include "StartWindow.h"

#include "StartPage.h"

#include <QVBoxLayout>

namespace UI {
    struct FileData;
}

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

    connect(startPage_, &StartPage::openFileByIDTriggered,
        this, &StartWindow::openFileByIDTriggered);
    connect(startPage_, &StartPage::openFileTriggered,
            this, &StartWindow::openFileTriggered);
    connect(startPage_, &StartPage::createFileTriggered,
            this, &StartWindow::createFileTriggered);
    connect(startPage_, &StartPage::renameFileTriggered,
            this, &StartWindow::renameFileTriggered);
    connect(startPage_, &StartPage::deleteFileTriggered,
            this, &StartWindow::deleteFileTriggered);

}


void UI::StartWindow::setFilesList(const QVector<ProjectData>& files) const {
    if (startPage_) {
        startPage_->setFilesList(files);
    }
}


void UI::StartWindow::setDefaultFilesPath(const QString& path) const {
    if (startPage_) {
        startPage_->setDefaultFilesPath(path);
    }
}
