#include "TopBarProject.h"

#include "HideOpenPanelButton.h"
#include "SettingsWidget.h"
#include "SideMenu.h"
#include "TabBar.h"

#include <QPushButton>


UI::TopBarProject::TopBarProject(QWidget* parent)
    : TopBar(parent) {
    setObjectName(QStringLiteral("TopBarProject"));
    createProjectButtons();
}


void UI::TopBarProject::setTabBar(TabBar* bar) {
    tabBar_ = bar;
}


void UI::TopBarProject::setLeftMenu(SideMenu* menu) {
    leftMenu_ = menu;
    if (leftMenu_) {
        leftMenu_->setObjectName(QStringLiteral("leftMenu"));
    }
}


void UI::TopBarProject::setRightMenu(SideMenu* menu) {
    rightMenu_ = menu;
    if (rightMenu_) {
        rightMenu_->setObjectName(QStringLiteral("rightMenu"));
    }
}


void UI::TopBarProject::createProjectButtons() {
    // Tabs toggle button
    btnTabs_ = createPanelButton();
    btnTabs_->setObjectName(QStringLiteral("btnTabs"));

    // Left panel toggle
    btnLeft_ = createPanelButton();
    btnLeft_->setObjectName(QStringLiteral("btnLeft"));

    // Right panel toggle
    btnRight_ = createPanelButton();
    btnRight_->setObjectName(QStringLiteral("btnRight"));

    // Settings button
    btnSettings_ = new QPushButton(QStringLiteral("⚙"), this);
    btnSettings_->setObjectName(QStringLiteral("btnSettings"));
    constexpr auto btnSize = QSize(32, 32);
    btnSettings_->setFixedSize(btnSize);
    btnSettings_->setFocusPolicy(Qt::NoFocus);
    btnSettings_->setStyleSheet(QStringLiteral(R"(
        QPushButton {
            background: transparent;
            color: white;
            border: none;
            border-radius: 6px;
        }
        QPushButton:hover {
            background: rgba(255,255,255,0.15);
        }
    )"));

    // Add buttons to layout
    auto* layout = getLayout();
    layout->insertWidget(0, btnTabs_);
    layout->insertWidget(1, btnLeft_);
    layout->insertWidget(2, btnRight_);
    layout->insertWidget(3, btnSettings_);

    // Connect signals
    connect(btnTabs_, &HideOpenPanelButton::toggled, this, [this]() {
        if (tabBar_) {
            tabBar_->setVisible(!tabBar_->isVisible());
        }
    });

    connect(btnLeft_, &HideOpenPanelButton::toggled, this, [this]() {
        if (leftMenu_) {
            leftMenu_->setVisible(!leftMenu_->isVisible());
        }
    });

    connect(btnRight_, &HideOpenPanelButton::toggled, this, [this]() {
        if (rightMenu_) {
            rightMenu_->setVisible(!rightMenu_->isVisible());
        }
    });

    connect(btnSettings_, &QPushButton::clicked, this, [this]() {
        auto *const settings = new SettingsWidget(this);
        settings->setAttribute(Qt::WA_DeleteOnClose);
        settings->setObjectName(QStringLiteral("settingsWidget"));
        settings->show();
    });
}
