#include "TopBarEditor.h"

#include <QEvent>

#include "CollaborationMenu.h"
#include "MenuButton.h"
#include "SettingsWidget.h"
#include "SideMenu.h"
#include "TabBar.h"
#include "VersionControlMenu.h"

#include <QToolButton>

#include "FileMenu.h"


UI::TopBarEditor::TopBarEditor(QWidget *parent)
    : BaseEditorTopBar(parent) {
    setObjectName(QStringLiteral("TopBarProject"));
    createProjectButtons();
    translate();
}


void UI::TopBarEditor::setTabBar(TabBar *bar) {
    if (!bar) {
        return;
    }
    tabBar_ = bar;
    btnTabs_ = createToggleButton(QIcon(":/Assets/icons/showHidePanels/tab.svg"),
                                  "",
                                  [this]() {
                                      if (tabBar_) {
                                          toggleWidgetVisibility(tabBar_);
                                      }
                                  });
    btnTabs_->setVisible(false);
}


void UI::TopBarEditor::setLeftMenu(SideMenu *menu) {
    if (!menu) {
        return;
    }
    setLeftPanel(menu);
}


void UI::TopBarEditor::setRightMenu(SideMenu *menu) {
    if (!menu) {
        return;
    }
    setRightPanel(menu);
}


void UI::TopBarEditor::setSettings(SettingsSystem* settings) {
    if (settings)
    settingsSystem_ = settings;
}

void UI::TopBarEditor::setConsole(QWidget *console) {
    if (!console) {
        return;
    }
    BaseEditorTopBar::setConsole(console);
}


void UI::TopBarEditor::setTabsButtonEnabled(const bool enabled) const {
    if (btnTabs_) {
        btnTabs_->setEnabled(enabled);
    }
}


void UI::TopBarEditor::setLeftButtonEnabled(const bool enabled) const {
    if (leftButton_) {
        leftButton_->setEnabled(enabled);
    }
}


void UI::TopBarEditor::setRightButtonEnabled(const bool enabled) const {
    if (rightButton_) {
        rightButton_->setEnabled(enabled);
    }
}


void UI::TopBarEditor::setPanelsButtonsEnabled(const bool enabled) const {
    setTabsButtonEnabled(enabled);
    setLeftButtonEnabled(enabled);
    setRightButtonEnabled(enabled);
    setConsoleButtonEnabled(enabled);
}


void UI::TopBarEditor::createProjectButtons() {
    projectMenu_ = new FileMenu(this);
    projectButton_ = new MenuButton("", projectMenu_, this);
    projectButton_->setDraw(true);
    addLeftWidget(projectButton_);

    connect(projectMenu_, &FileMenu::projectTriggered,
            this,               &TopBarEditor::projectTriggered);


    collaborationMenu_ = new CollaborationMenu(this);
    collabButton_ = new MenuButton("", collaborationMenu_, this);
    collabButton_->setDraw(true);
    addLeftWidget(collabButton_);
    collabButton_->setVisible(false);
    connect(collaborationMenu_, &CollaborationMenu::collabTriggered,
            this,               &TopBarEditor::collaborationTriggered);


    versionControlMenu_ = new VersionControlMenu(this);
    verButton_ = new MenuButton("", versionControlMenu_, this);
    verButton_->setDraw(true);


    addLeftWidget(verButton_);
    verButton_->setVisible(false);
    connect(versionControlMenu_, &VersionControlMenu::versionTriggered,
            this,               &TopBarEditor::versionTriggered);


    helpButton_ = new QPushButton(this);
    helpButton_->setObjectName(QStringLiteral("HelpButton"));
    helpButton_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    addLeftWidget(helpButton_);
    helpButton_->setVisible(false);

    setupSettings();
}


void UI::TopBarEditor::setupSettings() {
    btnSettings_ = new QPushButton(QStringLiteral("⚙"), this);
    btnSettings_->setObjectName(QStringLiteral("btnSettings"));
    constexpr auto btnSize = QSize(32, 32);
    btnSettings_->setFixedSize(btnSize);
    btnSettings_->setFocusPolicy(Qt::NoFocus);
    btnSettings_->setVisible(true);

    addCenterWidget(btnSettings_);
    connect(btnSettings_, &QPushButton::clicked, this, [this]() {
          auto* const w = new SettingsWidget(settingsSystem_, this);
          w->setAttribute(Qt::WA_DeleteOnClose);
          w->show();

          connect(w, &SettingsWidget::settingsChanged,
                  this, &TopBarEditor::settingsChanged);
      });
}


void UI::TopBarEditor::changeEvent(QEvent *e) {
    if (e && e->type() == QEvent::LanguageChange) {
        translate();
    }
    UI::BaseEditorTopBar::changeEvent(e);
}


void UI::TopBarEditor::translate() const {
    if (projectButton_) {
        projectButton_->setToolTip(UI::TopBarEditor::tr("File menu"));
        projectButton_->setText(UI::TopBarEditor::tr("File"));
    }
    if (verButton_) {
        verButton_->setToolTip(UI::TopBarEditor::tr("Version menu"));
        verButton_->setText(UI::TopBarEditor::tr("Version control"));
    }
    if (projectMenu_) {
        collabButton_->setToolTip(UI::TopBarEditor::tr("Collaboration menu"));
        collabButton_->setText(UI::TopBarEditor::tr("Collaboration"));
    }
    if (btnSettings_) {
        btnSettings_->setToolTip(UI::TopBarEditor::tr("Settings"));
    }
    if (helpButton_) {
        helpButton_->setToolTip(UI::TopBarEditor::tr("Help"));
        helpButton_->setText(UI::TopBarEditor::tr("Help"));
    }
    if (btnTabs_) {
        btnTabs_->setToolTip(UI::TopBarEditor::tr("Show/Hide tab panel"));
    }
}
