#include "TopBarLightEditor.h"

#include <QEvent>
#include <QPushButton>

#include "SettingsWidget.h"
#include "SettingsSystem.h"
#include "SideTool.h"

UI::TopBarLightEditor::TopBarLightEditor(QWidget* parent)
    : BaseEditorTopBar(parent) {
    createLightEditorWidgets();
    setObjectName(QStringLiteral("TopBarTab"));
    translate();
}

void UI::TopBarLightEditor::setDisplayName(const QString& name) {
    displayName_ = name;
    updateTitle();
}

void UI::TopBarLightEditor::updateTitle() const {
    if (title_) {
        title_->setText(displayName_.isEmpty() ? UI::TopBarLightEditor::tr("OurPaint") : displayName_);
    }
}

void UI::TopBarLightEditor::setLeftTool(SideTool* tool) {
    setLeftPanel(tool);
}

void UI::TopBarLightEditor::setRightTool(SideTool* tool) {
    setRightPanel(tool);
}

void UI::TopBarLightEditor::setConsole(QWidget* console) {
    BaseEditorTopBar::setConsole(console);
}

void UI::TopBarLightEditor::setSettings(SettingsSystem *settings) {
    if (settings) {
        settingsSystem_ = settings;
    }
}

void UI::TopBarLightEditor::createLightEditorWidgets() {
    title_ = new QLabel((""), this);
    title_->setObjectName(QStringLiteral("TitleOurPaint"));

    returnBtn_ = new QPushButton(this);
    connect(returnBtn_, &QPushButton::clicked, this, &TopBarLightEditor::returnLightEditorWindowTriggered);
    returnBtn_->setObjectName("ReturnTab");
    returnBtn_->setIcon(QIcon(":/Assets/icons/showHidePanels/home.svg"));

    addLeftWidget(title_);
    addLeftWidget(returnBtn_);

    // Добавляем настройки
    setupSettings();
}

void UI::TopBarLightEditor::setupSettings() {
    btnSettings_ = new QPushButton(QStringLiteral("⚙"), this);
    btnSettings_->setObjectName(QStringLiteral("btnSettings"));
    constexpr auto btnSize = QSize(32, 32);
    btnSettings_->setFixedSize(btnSize);
    btnSettings_->setFocusPolicy(Qt::NoFocus);
    btnSettings_->setVisible(true);

    addCenterWidget(btnSettings_);

    connect(btnSettings_, &QPushButton::clicked, this, [this]() {
        if (settingsWidget_ && settingsWidget_->isVisible()) {
            settingsWidget_->raise();
            settingsWidget_->activateWindow();
            return;
        }

        if (!settingsWidget_) {
            settingsWidget_ = new SettingsWidget(settingsSystem_, this);
            settingsWidget_->setAttribute(Qt::WA_DeleteOnClose);

            connect(settingsWidget_, &QObject::destroyed, this, [this]() {
                settingsWidget_ = nullptr;
            });

            connect(settingsWidget_, &SettingsWidget::settingsChanged,
                    this, &TopBarLightEditor::settingsChanged);
        }

        settingsWidget_->show();
        settingsWidget_->raise();
        settingsWidget_->activateWindow();
    });
}

void UI::TopBarLightEditor::changeEvent(QEvent* e) {
    if (e && e->type() == QEvent::LanguageChange) {
        translate();
    }
    QWidget::changeEvent(e);
}

void UI::TopBarLightEditor::translate() const {
    if (returnBtn_) {
        returnBtn_->setToolTip(UI::TopBarLightEditor::tr("Return"));
    }
    if (btnSettings_) {
        btnSettings_->setToolTip(UI::TopBarLightEditor::tr("Settings"));
    }
}