#include "BaseEditorTopBar.h"

#include <qcoreapplication.h>
#include <QLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QTimer>

namespace {
    const auto CONSOLE_ICON_PATH = QStringLiteral(":/Assets/icons/showHidePanels/console.svg");
    const auto LEFT_ICON_PATH = QStringLiteral(":/Assets/icons/showHidePanels/left.svg");
    const auto RIGHT_ICON_PATH = QStringLiteral(":/Assets/icons/showHidePanels/right.svg");
} // namespace


UI::BaseEditorTopBar::BaseEditorTopBar(QWidget* parent)
    : TopBar(parent) {
    translate();
}


void UI::BaseEditorTopBar::setConsoleButtonEnabled(const bool enabled) const {
    if (consoleButton_) {
        consoleButton_->setEnabled(enabled);
    }
}


void UI::BaseEditorTopBar::toggleWidgetVisibility(QWidget* widget) {
    if (!widget) {
        return;
    }

    const bool newVisible = !widget->isVisible();
    widget->setVisible(newVisible);
}


void UI::BaseEditorTopBar::setLeftPanel(QWidget* panel) {
    setLeftPanel(panel, QIcon(LEFT_ICON_PATH),"");
}


void UI::BaseEditorTopBar::setRightPanel(QWidget* panel) {
    setRightPanel(panel, QIcon(RIGHT_ICON_PATH), "");
}


void UI::BaseEditorTopBar::setConsole(QWidget* consoleWidget) {
    setCommandConsoleWidget(consoleWidget);
}


void UI::BaseEditorTopBar::setLeftPanel(QWidget* panel, const QIcon& icon, const QString& toolTip) {
    leftPanel_ = panel;
    if (!leftButton_) {
        leftButton_ = createToggleButton(icon, toolTip, [this]() {
            if (leftPanel_) {
                toggleWidgetVisibility(leftPanel_);
            }
        });
    }
}


void UI::BaseEditorTopBar::setRightPanel(QWidget* panel, const QIcon& icon, const QString& toolTip) {
    rightPanel_ = panel;
    if (!rightButton_) {
        rightButton_ = createToggleButton(icon, toolTip, [this]() {
            if (rightPanel_) {
                toggleWidgetVisibility(rightPanel_);
            }
        });
    }
}


void UI::BaseEditorTopBar::setCommandConsoleWidget(QWidget* consoleWidget) {
    commandConsoleWidget_ = consoleWidget;
    if (!consoleButton_) {
        consoleButton_ = createToggleButton(QIcon(CONSOLE_ICON_PATH),"", [this]() {
            if (commandConsoleWidget_) {
                toggleWidgetVisibility(commandConsoleWidget_);
            }
        });
    }
}


QPushButton* UI::BaseEditorTopBar::createToggleButton(const QIcon& icon,
                                                      const QString& toolTip,
                                                      const std::function<void()>& onClick) {
    auto* button = createWindowButton(icon);
    button->setToolTip(toolTip);
    addCenterWidget(button);

    connect(button, &QPushButton::clicked, this, onClick);

    return button;
}

void UI::BaseEditorTopBar::changeEvent(QEvent* e) {
    if (e && e->type() == QEvent::LanguageChange) {
        translate();
    }
    QWidget::changeEvent(e);
}

void UI::BaseEditorTopBar::translate() const {
    if (leftButton_) {
        leftButton_->setToolTip(UI::BaseEditorTopBar::tr("Show/Hide left panel"));
    }
    if (rightButton_) {
        rightButton_->setToolTip(UI::BaseEditorTopBar::tr("Show/Hide right panel"));
    }
    if (consoleButton_) {
        consoleButton_->setToolTip(UI::BaseEditorTopBar::tr("Show/Hide concole"));
    }
}
