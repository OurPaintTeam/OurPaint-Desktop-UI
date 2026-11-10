#include "StartPage.h"

#include <QEvent>

#include "AnimationArea.h"
#include "InputWidget.h"
#include "FilesListWidget.h"
#include "TopBarStart.h"

#include <QFileDialog>
#include <qguiapplication.h>
#include <QHBoxLayout>
#include <QScreen>

#include "PathInputWidget.h"


UI::StartPage::StartPage(QWidget* parent)
    : BasePage(parent) {
    setObjectName("StartPage");
    initUI();
    translate();

    connect(filesListWidget_, &FilesListWidget::pressFileTriggered, this, [this](qint64 id) {
    emit openFileByIDTriggered(id,false);
    });

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
    auto* prompt = new PathInputWidget(tr("File path:"), this, UI::PathMode::Ourp);
    prompt->setDefaultText(defaultFilesPath_);

    const QSize size = prompt->sizeHint();
    const QPoint centerGlobal = mapToGlobal(rect().center());
    const QPoint target(centerGlobal.x() - size.width()  / 2,
                        centerGlobal.y() - size.height() / 2);

    QScreen* screen = QGuiApplication::screenAt(centerGlobal);
    if (!screen) screen = QGuiApplication::primaryScreen();
    const QRect avail = screen->availableGeometry();

    int x = target.x();
    int y = target.y();
    if (x + size.width()  > avail.right())  x = avail.right()  - size.width();
    if (y + size.height() > avail.bottom()) y = avail.bottom() - size.height();
    if (x < avail.left()) x = avail.left();
    if (y < avail.top())  y = avail.top();

    prompt->move(x, y);


    connect(prompt, &InputWidget::inputEnteredTriggered, this,
            [this, prompt](const QString& text) {
                const bool newWindow = false;
                if (text.isEmpty()) return;
                emit openFileTriggered(text, newWindow);
            });

    prompt->show();
    prompt->raise();
    prompt->activateWindow();
}


void UI::StartPage::onCreateFileSlot() {
    auto* prompt = new PathInputWidget(tr("Folder Path:"), this, UI::PathMode::Folder);
    prompt->setDefaultText(defaultFilesPath_);

    const QSize size = prompt->sizeHint();

    const QPoint centerGlobal = mapToGlobal(rect().center());
    const QPoint target(centerGlobal.x() - size.width()  / 2,
                        centerGlobal.y() - size.height() / 2);

    QScreen* screen = QGuiApplication::screenAt(centerGlobal);
    if (!screen) screen = QGuiApplication::primaryScreen();
    const QRect avail = screen->availableGeometry();

    int x = target.x();
    int y = target.y();
    if (x + size.width()  > avail.right())  x = avail.right()  - size.width();
    if (y + size.height() > avail.bottom()) y = avail.bottom() - size.height();
    if (x < avail.left()) x = avail.left();
    if (y < avail.top())  y = avail.top();

    prompt->move(x, y);


    connect(prompt, &InputWidget::inputEnteredTriggered, this,
            [this, prompt](const QString& text) {
                const bool newWindow = false;
                if (text.isEmpty()) return;
                emit createFileTriggered(text, newWindow);
            });

    prompt->show();
    prompt->raise();
    prompt->activateWindow();
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
