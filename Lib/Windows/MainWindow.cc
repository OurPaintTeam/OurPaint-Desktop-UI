#include "MainWindow.h"
#include "StartPage.h"
#include "EditorPage.h"
#include <QCloseEvent>
#include <QVBoxLayout>


UI::MainWindow::MainWindow(QWidget *parent)
    : BaseEditorWindow(parent) {
    initUI();
}


void UI::MainWindow::initUI() {
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    stack_ = new QStackedWidget(central);
    layout->addWidget(stack_);
    setCentralWidget(central);

    startPage_ = new StartPage(stack_);
    editorPageWidget_ = new EditorPage(stack_);
    editorPage_ = editorPageWidget_;

    stack_->addWidget(startPage_);
    stack_->addWidget(editorPageWidget_);
    stack_->setCurrentWidget(startPage_);

    connect(startPage_, &StartPage::openFileByIDTriggered, this, &MainWindow::openFileByIDTriggered);
    connect(startPage_, &StartPage::openFileTriggered, this, &MainWindow::openFileTriggered);
    connect(startPage_, &StartPage::createFileTriggered, this, &MainWindow::createFileTriggered);
    connect(startPage_, &StartPage::renameFileTriggered, this, &MainWindow::renameFileTriggered);
    connect(startPage_, &StartPage::deleteFileTriggered, this, &MainWindow::deleteFileTriggered);

    connect(editorPageWidget_, &EditorPage::goToStartWindowTriggered, this, &MainWindow::goToStartWindowTriggered);
    connect(editorPageWidget_, &EditorPage::openFileTriggered, this, &MainWindow::openFileTriggered);
    connect(editorPageWidget_, &EditorPage::createFileTriggered, this, &MainWindow::createFileTriggered);
    connect(editorPageWidget_, &EditorPage::renameTabTriggered, this, &MainWindow::renameTabTriggered);
    connect(editorPageWidget_, &EditorPage::setActiveTabTriggered, this, &MainWindow::setActiveTabTriggered);
    connect(editorPageWidget_, &EditorPage::removeTabTriggered, this, &MainWindow::removeTabTriggered);
    connect(editorPageWidget_, &EditorPage::closeTabTriggered, this, &MainWindow::closeTabTriggered);
    connect(editorPageWidget_, &EditorPage::openLightEditorWindowTriggered, this,
            &MainWindow::openLightEditorWindowTriggered);
    connect(editorPageWidget_, &EditorPage::settingsChanged, this, &MainWindow::settingsChanged);
    connect(editorPageWidget_, &EditorPage::scriptTriggered, this, &MainWindow::onOpenScriptTriggered);
    connect(editorPageWidget_, &EditorPage::closeApplicationTriggered, this, &MainWindow::closeApplicationSlot);

    connect(editorPageWidget_, &BaseEditorPage::sentCommandTriggered, this, &BaseEditorWindow::sentCommandTriggered);
    connect(editorPageWidget_, &BaseEditorPage::primitiveTriggered, this, &BaseEditorWindow::primitiveTriggered);
    connect(editorPageWidget_, &BaseEditorPage::constraintTriggered, this, &BaseEditorWindow::constraintTriggered);
    connect(editorPageWidget_, &BaseEditorPage::toolsTriggered, this, &BaseEditorWindow::toolsTriggered);

    connect(this, &UI::FramelessWindow::ourpFileDropped, this,
        [this](const QString& path) {
            emit openFileTriggered(path, false);
        });
}

void UI::MainWindow::setNotificationsEnabled(const bool enable) {
   includeNotificationManager(enable);
}

void UI::MainWindow::setFilesList(const QVector<ProjectData>& files) const {
    if (startPage_) startPage_->setFilesList(files);
}


void UI::MainWindow::setDefaultFilesPath(const QString& path) const {
    if (startPage_) startPage_->setDefaultFilesPath(path);
}


void UI::MainWindow::setDefaultProjectPath(const QString& path) {
    if (editorPageWidget_) editorPageWidget_->setDefaultProjectPath(path);
}


void UI::MainWindow::setDefaultScriptPath(const QString& path) {
    if (editorPageWidget_) editorPageWidget_->setDefaultScriptPath(path);
}


void UI::MainWindow::addTabSlot(const FileData& data) const {
    if (editorPageWidget_) editorPageWidget_->onAddTabSlot(data);
}


void UI::MainWindow::deleteTabSlot(const qint64 id) const {
    if (editorPageWidget_) editorPageWidget_->onDeleteTabSlot(id);
}


void UI::MainWindow::renameTabSlot(FileData& data) const {
    if (editorPageWidget_) {
        editorPageWidget_->onRenameTabSlot(data);
        editorPageWidget_->setActiveID(data.id);
    }
}


void UI::MainWindow::showStartPage() {
    if (stack_ && startPage_) stack_->setCurrentWidget(startPage_);
}


void UI::MainWindow::showEditorPage() {
    if (stack_ && editorPageWidget_) stack_->setCurrentWidget(editorPageWidget_);
}


QStackedWidget* UI::MainWindow::stack() const { return stack_; }
UI::StartPage* UI::MainWindow::startPage() const { return startPage_; }
UI::EditorPage* UI::MainWindow::editorPage() const { return editorPageWidget_; }


void UI::MainWindow::closeEvent(QCloseEvent *event) {
    emit closeApplicationTriggered();
    if (event) event->accept();
}


void UI::MainWindow::closeApplicationSlot() {
    emit closeApplicationTriggered();
    close();
}