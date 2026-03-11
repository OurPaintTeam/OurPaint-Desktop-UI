#include "CustomMenu.h"


UI::CustomMenu::CustomMenu(QWidget* parent)
    : QMenu(parent) {
    setupWindow();
    setObjectName(QStringLiteral("CustomMenu"));
}


void UI::CustomMenu::setupWindow() {
    // Remove window frame
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

    // Enable transparent background
    setAttribute(Qt::WA_TranslucentBackground, true);

    // Menu style
    setStyleSheet(R"(
        QMenu {
            background-color: #3f3e46;
            color: #D8D8F6;
            border: none;
            padding: 4px;
            border-radius: 6px;
        }

        QMenu::item {
            background: transparent;
            padding: 6px 24px 6px 20px;
        }

        QMenu::item:selected {
            background: rgba(255,255,255,0.12);
            border-radius: 6px;
        }

        QMenu::separator {
            height: 1px;
            background: rgba(255,255,255,0.15);
            margin: 6px 8px;
        }
    )");
}
