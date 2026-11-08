#include "StartPage.h"

#include <QEvent>

#include "AnimationArea.h"
#include "InputWidget.h"
#include "FilesListWidget.h"
#include "TopBarStart.h"

#include <QFileDialog>
#include <QHBoxLayout>

#include "PathInputWidget.h"


UI::StartPage::StartPage(QWidget* parent)
    : BasePage(parent) {
    setObjectName("StartPage");
    initUI();
    translate();
    connect(filesListWidget_, &FilesListWidget::pressFileTriggered, this,
            &StartPage::pressFileTriggered);
    connect(filesListWidget_, &FilesListWidget::renameFileTriggered, this, &StartPage::renameFileTriggered);
    connect(filesListWidget_, &FilesListWidget::deleteFileTriggered, this, &StartPage::deleteFileTriggered);

    connect(filesListWidget_, &FilesListWidget::pressOpenButtonTriggered, this, &StartPage::onOpenFileSlot);
    connect(filesListWidget_, &FilesListWidget::pressCreateButtonTriggered, this,
            &StartPage::onCreateFileSlot);
}


void UI::StartPage::setFilesList(const QVector<ProjectData>& files) const {
    filesListWidget_->setFiles(files);
}


void UI::StartPage::setDefaultFilesPath(const QString& path){
    defaultFilesPath_ = path;
}


void UI::StartPage::onOpenFileSlot() {
    const auto folderPath = QFileDialog::getExistingDirectory(
        this,
        tr("Open File"),
        defaultFilesPath_,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (folderPath.isEmpty()) {
        return;
    }

    emit openFileInThisWindowTriggered(folderPath);
}


void UI::StartPage::onCreateFileSlot() {
    auto* prompt = new PathInputWidget(tr("File path:"), this);
    const auto rect = this->rect();
    const auto size = prompt->sizeHint();

    prompt->move(rect.center().x() - size.width() / 2,
                 rect.center().y() - size.height() / 2);

    prompt->setDefaultText(defaultFilesPath_);
    prompt->show();

    connect(prompt, &InputWidget::inputEnteredTriggered, this, [this,prompt](const QString& text) {
        if (text.isEmpty()) {
            return;
        }
        prompt->deleteLater();

        emit createFileInThisWindowTriggered(text);
    });
}


void UI::StartPage::initUI() {
    initBasePage();
    contentLayout_ = new QHBoxLayout(mainArea_);

    topBar_ = new TopBarStart(this);

    filesListWidget_ = new FilesListWidget(mainArea_);
    animationArea_ = new AnimationArea(mainArea_);

    contentLayout_->setContentsMargins(0, 0, 0, 0);
    contentLayout_->setSpacing(0);

    mainLayout_->insertWidget(0, topBar_);

    contentLayout_->addWidget(filesListWidget_);
    contentLayout_->addWidget(animationArea_, 1);
}

void UI::StartPage::changeEvent(QEvent* e) {
    if (e && e->type() == QEvent::LanguageChange) {
        translate();
    }
    QWidget::changeEvent(e);
}

void UI::StartPage::translate() const {
}
