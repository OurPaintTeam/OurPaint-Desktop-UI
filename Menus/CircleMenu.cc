#include "CircleMenu.h"

UI::CircleMenu::CircleMenu(QWidget* parent)
    : UI::CustomMenu(parent)
{
    circleCenter_  = addAction("Circle by center");
    circleTwoPoints_  = addAction("Circle by 2 points");
    circleThreePoints_  = addAction("Circle by 3 points");
    addSeparator();
    circleSettings_  = addAction("Circle settings...");

    setObjectName(QStringLiteral("CircleMenu"));
}
