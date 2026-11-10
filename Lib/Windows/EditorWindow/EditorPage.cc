#include "EditorPage.h"

#include <QEvent>
#include <QFileDialog>
#include <qguiapplication.h>
#include <QIcon>
#include <QPushButton>
#include <QScreen>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>

#include "ContainerWidget.h"
#include "InputWidget.h"
#include "NameInputWidget.h"
#include "NavigationWidget.h"
#include "PathInputWidget.h"
#include "SideMenu.h"
#include "TabBar.h"
#include "ToolBar.h"
#include "ToolsType.h"
#include "TopBarEditor.h"


namespace {
    constexpr int MARGIN = 0;
    constexpr int SPACING = 0;
} // namespace


UI::EditorPage::EditorPage(QWidget* parent)
    : BaseEditorPage(parent) {
    setObjectName("ProjectPage");

    initUI();
    setupConnections();
}


void UI::EditorPage::setDefaultProjectPath(const QString& path) {
    defaultProjectPath_ = path;
}

void UI::EditorPage::setDefaultScriptPath(const QString& path) {
    defaultScriptPath_ = path;
}


void UI::EditorPage::initUI() {
    initBaseEditorPage();
    createTopBar();
    createSideColumns();
    createCenterColumn();
    createWorkspace();
    createNavigation();
    createSideContainers();
    assembleLayout();
    updatePageSlot();
}


void UI::EditorPage::setupConnections() {
    connect(tabBar_, &TabBar::openLightEditorWindowTriggered, this, &EditorPage::openLightEditorWindowTriggered);
    connect(tabBar_, &TabBar::setActiveTabTriggered, this, [this](const qint64 id) {
        setActiveID(id);
        emit setActiveTabTriggered(id);
    });
    connect(tabBar_, &TabBar::createTabTriggered, this, [this](const QString& name) {
    emit createFileTriggered(name,false);
    });
    connect(tabBar_, &TabBar::allTabsCloseTriggered, this, &EditorPage::updatePageSlot);
    connect(tabBar_, &TabBar::renameTabTriggered, this, &EditorPage::renameTabTriggered);
    connect(tabBar_, &TabBar::removeTabTriggered, this, &EditorPage::removeTabTriggered);
    connect(tabBar_, &TabBar::closeTabTriggered, this, &EditorPage::closeTabTriggered);

    connect(navigationPage_, &NavigationWidget::closeApplicationTriggered, this,
            &EditorPage::closeApplicationTriggered);
    connect(navigationPage_, &NavigationWidget::goToStartWindowTriggered, this,
            &EditorPage::goToStartWindowTriggered);
    connect(navigationPage_, &NavigationWidget::createFileTriggered, this,
            &EditorPage::createFileSlot);

    connect(infoButton_, &QPushButton::clicked, this, &EditorPage::openInformationPanel);
    connect(messengerButton_, &QPushButton::clicked, this, &EditorPage::openMessengerPanel);

    connect(topBar_, &TopBarEditor::projectTriggered,
            this,    &EditorPage::onProjectAction);

    connect(topBar_, &TopBarEditor::collaborationTriggered,
            this, &EditorPage::collaborationTriggered);
    connect(topBar_, &TopBarEditor::versionTriggered,
            this, &EditorPage::versionTriggered);


  connect(topBar_, &TopBarEditor::settingsChanged,
        this, &EditorPage::settingsChanged);
}


void UI::EditorPage::onProjectAction(UI::projectMenuType p)
{
    switch (p) {
        case UI::projectMenuType::createFile: createFileSlot();        break;
        case UI::projectMenuType::openFile:   openFileSlot();          break;
        case UI::projectMenuType::exportFile: emit exportFileTriggered(); break;
        case UI::projectMenuType::scriptFile: onOpenScriptSlot();      break;
    }
}

void UI::EditorPage::createTopBar() {
    topBar_ = new TopBarEditor(this);
    mainLayout_->insertWidget(0, topBar_);
}

void UI::EditorPage::setSettings(SettingsSystem* settings) const{
    if (topBar_) topBar_->setSettings(settings);
}

void UI::EditorPage::createSideColumns() {
    leftColumn_  = new SideMenu(mainArea_);
    rightColumn_ = new SideMenu(mainArea_);
}


void UI::EditorPage::createCenterColumn() {
    centerWidget_ = new QWidget(mainArea_);

    centerLayout_ = new QVBoxLayout(centerWidget_);
    centerLayout_->setContentsMargins(MARGIN, MARGIN, MARGIN, MARGIN);
    centerLayout_->setSpacing(SPACING);

    tabBar_ = new TabBar(centerWidget_);
    centerStack_ = new QStackedWidget(centerWidget_);

    centerLayout_->addWidget(tabBar_);
    centerLayout_->addWidget(centerStack_, 1);
}


void UI::EditorPage::createWorkspace() {
    workspacePage_ = createWorkspacePage(centerStack_);
    centerStack_->addWidget(workspacePage_);

    topBar_->setTabBar(tabBar_);
    topBar_->setLeftMenu(leftColumn_);
    topBar_->setRightMenu(rightColumn_);
    topBar_->setConsole(consoleWrapper_);
}


void UI::EditorPage::createNavigation() {
    navigationPage_ = new NavigationWidget(centerWidget_);
    centerStack_->addWidget(navigationPage_);
}


void UI::EditorPage::createSideContainers() {
    infoContainer_ = new ContainerWidget(ContainerType::Side, Qt::Vertical);
    infoButton_ = infoContainer_->addButton(QIcon(":/Assets/icons/panel/info.png"));
    infoButton_->setObjectName("InfoButton");

    messengerContainer_ = new ContainerWidget(ContainerType::Side, Qt::Vertical);
    messengerButton_ = messengerContainer_->addButton(QIcon(":/Assets/icons/panel/message.png"));
    messengerButton_->setObjectName("MesButton");

    leftColumn_->addContainer(infoContainer_);
    leftColumn_->addContainer(messengerContainer_);
}


void UI::EditorPage::assembleLayout() const {
    rootLayout_->addWidget(leftColumn_);
    rootLayout_->addWidget(centerWidget_, 1);
    rootLayout_->addWidget(rightColumn_);

    leftColumn_->hide();
    rightColumn_->hide();
    //tabBar_->hide();
}



void UI::EditorPage::updatePageSlot() const {
    if (tabBar_->isEmptyTab()) {
        centerStack_->setCurrentWidget(navigationPage_);
        toolBar_->hideAllElements();
        topBar_->setConsoleButtonEnabled(false);
        return;
    }

    centerStack_->setCurrentWidget(workspacePage_);
    toolBar_->showAllElements();
    topBar_->setConsoleButtonEnabled(true);
}


void UI::EditorPage::openInformationPanel() const {
    if (infoContainer_->currentSlot() == leftColumn_->slotWidget()) {
        leftColumn_->openInfPanel();
    } else {
        rightColumn_->openInfPanel();
    }
}


void UI::EditorPage::openMessengerPanel() const {
    if (messengerContainer_->currentSlot() == leftColumn_->slotWidget()) {
        leftColumn_->openMesPanel();
    } else {
        rightColumn_->openMesPanel();
    }
}



void UI::EditorPage::onOpenScriptSlot() {
    auto* prompt = new PathInputWidget(tr("Script Path:"), this, UI::PathMode::Txt);
    prompt->setDefaultText(defaultScriptPath_);

    prompt->setAttribute(Qt::WA_DeleteOnClose, true);

    const QSize  size      = prompt->sizeHint();
    const QPoint globalPos = QCursor::pos();

    QScreen* screen = QGuiApplication::screenAt(globalPos);
    if (!screen) screen = QGuiApplication::primaryScreen();
    const QRect avail = screen->availableGeometry();

    int x = globalPos.x();
    int y = globalPos.y();
    if (x + size.width()  > avail.right())  x = avail.right()  - size.width();
    if (y + size.height() > avail.bottom()) y = avail.bottom() - size.height();
    if (x < avail.left()) x = avail.left();
    if (y < avail.top())  y = avail.top();

    prompt->move(x, y);
    prompt->show();
    prompt->raise();
    prompt->activateWindow();

    connect(prompt, &InputWidget::inputEnteredTriggered, this,
            [this, prompt](const QString& text) {
                prompt->deleteLater();
                if (text.isEmpty()) return;
                emit scriptTriggered(text);
            });
}


void UI::EditorPage::openFileSlot() {
    auto* prompt = new PathInputWidget(tr("File Path:"), this, UI::PathMode::Ourp);
    prompt->setDefaultText(defaultProjectPath_);
    prompt->setCheckBoxQuestion(tr("Open in new window"));

    prompt->setAttribute(Qt::WA_DeleteOnClose, true);

    const QSize  size      = prompt->sizeHint();
    const QPoint globalPos = QCursor::pos();

    QScreen* screen = QGuiApplication::screenAt(globalPos);
    if (!screen) screen = QGuiApplication::primaryScreen();
    const QRect avail = screen->availableGeometry();

    int x = globalPos.x();
    int y = globalPos.y();
    if (x + size.width()  > avail.right())  x = avail.right()  - size.width();
    if (y + size.height() > avail.bottom()) y = avail.bottom() - size.height();
    if (x < avail.left()) x = avail.left();
    if (y < avail.top())  y = avail.top();

    prompt->move(x, y);
    prompt->show();
    prompt->raise();
    prompt->activateWindow();


    connect(prompt, &InputWidget::inputEnteredTriggered, this,
            [this, prompt](const QString& text) {
                const bool newWindow = prompt->isCheckBoxChecked();
                prompt->close();
                if (text.isEmpty()) return;
                emit openFileTriggered(text, newWindow);
            });
}



void UI::EditorPage::createFileSlot() {
    auto* prompt = new PathInputWidget(tr("File Name:"), this, UI::PathMode::Folder);
    prompt->setDefaultText(defaultProjectPath_);
    prompt->setCheckBoxQuestion(tr("Open in new window"));

    prompt->setAttribute(Qt::WA_DeleteOnClose, true);

    const QSize  size      = prompt->sizeHint();
    const QPoint globalPos = QCursor::pos();

    QScreen* screen = QGuiApplication::screenAt(globalPos);
    if (!screen) screen = QGuiApplication::primaryScreen();
    const QRect avail = screen->availableGeometry();

    int x = globalPos.x();
    int y = globalPos.y();
    if (x + size.width()  > avail.right())  x = avail.right()  - size.width();
    if (y + size.height() > avail.bottom()) y = avail.bottom() - size.height();
    if (x < avail.left()) x = avail.left();
    if (y < avail.top())  y = avail.top();

    prompt->move(x, y);
    prompt->show();
    prompt->raise();
    prompt->activateWindow();


    connect(prompt, &InputWidget::inputEnteredTriggered, this,
            [this, prompt](const QString& text) {
                const bool newWindow = prompt->isCheckBoxChecked();
                prompt->close();
                if (text.isEmpty()) return;
                emit createFileTriggered(text, newWindow);
            });
}

void UI::EditorPage::onAddTabSlot(const FileData& data) const {
    tabBar_->addTabSlot(data);
    updatePageSlot();
}


void UI::EditorPage::onDeleteTabSlot(const qint64 id) const {
    tabBar_->deleteTabSlot(id);
    updatePageSlot();
}


void UI::EditorPage::onRenameTabSlot(FileData& data) const {
    tabBar_->renameTab(data);
}

void UI::EditorPage::changeEvent(QEvent* e) {
    if (e && e->type() == QEvent::LanguageChange) {
        translate();
    }
    BaseEditorPage::changeEvent(e);
}

void UI::EditorPage::translate() const {
    if (infoButton_) {
        infoButton_->setToolTip(UI::EditorPage::tr("Information menu"));
    }
    if (messengerButton_) {
        messengerButton_->setToolTip(tr("Messenger"));
    }
}

