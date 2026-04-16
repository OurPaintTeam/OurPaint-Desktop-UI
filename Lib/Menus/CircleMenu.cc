#include "CircleMenu.h"

#include "ToolsType.h"


UI::CircleMenu::CircleMenu(QWidget* parent)
    : UI::CustomMenu(parent) {
    circleCenter_ = addAction(QIcon(":/Assets/icons/primitives/circleByDiameter.png"), "Circle by diameter");
    circleCenter_->setData(QVariant::fromValue(PrimitiveType::CircleByDiameter));

    circleTwoPoints_ = addAction(QIcon(":/Assets/icons/primitives/circleTwoPoints.svg"), "Circle by 2 points");
    circleTwoPoints_->setData(QVariant::fromValue(PrimitiveType::CircleTwoPoints));

    circleThreePoints_ = addAction(QIcon(":/Assets/icons/primitives/ellipseThreePoints.svg"), "Ellipse by 3 points");
    circleThreePoints_->setData(QVariant::fromValue(PrimitiveType::EllipseThreePoints));

    addSeparator();
    circleSettings_ = addAction(QIcon(":/Assets/icons/tools/settings.svg"), "Circle settings...");
    circleSettings_->setData(QVariant::fromValue(PrimitiveType::CircleSettings));

    for (auto* a: actions()) {
        if (!a->isSeparator()) {
            connect(a, &QAction::triggered, this, [this, a]() { emit actionSelected(a); });
        }
    }

    setObjectName(QStringLiteral("CircleMenu"));
    setAttribute(Qt::WA_StyledBackground, true);
}
