#include "EditorPage.h"

#include <QEvent>
#include <QFileDialog>
#include <QIcon>
#include <QPushButton>
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


void UI::EditorPage::setDefaultPath(const QString& path) {
    defaultPath_ = path;
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

void UI::EditorPage::createFileSlot() {
    tabBar_->createTabSlot();
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
    auto* prompt = new PathInputWidget("Script Path:", this,true);
    const auto rect = this->rect();
    const auto size = prompt->sizeHint();

    prompt->setDefaultText(defaultPath_);
    prompt->move(rect.center().x() - size.width() / 2,
                 rect.center().y() - size.height() / 2);
    prompt->show();

    connect(prompt, &InputWidget::inputEnteredTriggered, this, [this, prompt](const QString& text) {
        if (text.isEmpty()) {
            return;
        }

        prompt->deleteLater();
        emit scriptTriggered(text);
    });
}


void UI::EditorPage::openFileSlot() {
    const auto filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        defaultPath_,
        tr("Files (*.ourp);;All Files (*)")
    );

    if (filePath.isEmpty()) {
        return;
    }

    emit openFileTriggered(filePath,false);
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
    QWidget::changeEvent(e);
}

void UI::EditorPage::translate() const {
    if (infoButton_) {
        infoButton_->setToolTip(UI::EditorPage::tr("Information menu"));
    }
    if (messengerButton_) {
        messengerButton_->setObjectName(UI::EditorPage::tr("Messanger"));
    }
}

