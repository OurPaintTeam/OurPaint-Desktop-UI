#include "ArcMenu.h"
#include <QAction>

UI::ArcMenu::ArcMenu(QWidget* parent)
    : UI::CustomMenu(parent)
{
    arcCenter_  = addAction("A1");
    arc3Points_ = addAction("A3");
    arcRadius_  = addAction("AR");
    addSeparator();
    arcSettings_ = addAction("Settings");

    // первая кнопка по умолчанию
    defaultAction_ = arcCenter_;

    connect(this, &QMenu::triggered, this, [this](QAction* action) {
        emit actionSelected(action);
    });

    setObjectName(QStringLiteral("ArcMenu"));
}

QAction* UI::ArcMenu::defaultAction() const
{
    return defaultAction_;
}
