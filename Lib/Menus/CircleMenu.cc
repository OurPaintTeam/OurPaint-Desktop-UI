#include "CircleMenu.h"

#include "ToolsType.h"


UI::CircleMenu::CircleMenu(QWidget* parent)
    : UI::CustomMenu(parent) {

    // Radius
    circleByRadius_ = addAction(QIcon(":/Assets/icons/primitives/circleByRadius.svg"), "Circle by radius");
    circleByRadius_->setData(QVariant::fromValue(PrimitiveType::CircleByRadius));

    // Diameter
    circleByDiameter_ = addAction(QIcon(":/Assets/icons/primitives/circleByDiameter.svg"), "Circle by diameter");
    circleByDiameter_->setData(QVariant::fromValue(PrimitiveType::CircleByDiameter));

    // Two points
    circleByTwoPoints_ = addAction(QIcon(":/Assets/icons/primitives/circleByDiameter.svg"), "Circle by two points");
    circleByTwoPoints_->setData(QVariant::fromValue(PrimitiveType::CircleByTwoPoints));

    // Three points
    circleByThreePoints_ = addAction(QIcon(":/Assets/icons/primitives/circleByDiameter.svg"), "Circle by three points");
    circleByThreePoints_->setData(QVariant::fromValue(PrimitiveType::CircleByThreePoints));

    // Tangent two lines
    circleTangentTwoLines_ = addAction(QIcon(":/Assets/icons/primitives/circleByDiameter.svg"), "Circle tangent two lines");
    circleTangentTwoLines_->setData(QVariant::fromValue(PrimitiveType::CircleTangentTwoLines));

    // Tangent three lines
    circleTangentThreeLines_ = addAction(QIcon(":/Assets/icons/primitives/circleByDiameter.svg"), "Circle tangent three lines");
    circleTangentThreeLines_->setData(QVariant::fromValue(PrimitiveType::CircleTangentThreeLines));


    addSeparator();
    // circleSettings_ = addAction(QIcon(":/Assets/icons/tools/settings.svg"), "Circle settings...");
    // circleSettings_->setData(QVariant::fromValue(PrimitiveType::CircleSettings));

    for (auto* a: actions()) {
        if (!a->isSeparator()) {
            connect(a, &QAction::triggered, this, [this, a]() { emit actionSelected(a); });
        }
    }

    setObjectName(QStringLiteral("CircleMenu"));
    setAttribute(Qt::WA_StyledBackground, true);
}

QAction* UI::CircleMenu::circleByRadius() const {
    return circleByRadius_;
}

QAction* UI::CircleMenu::circleByDiameter() const {
    return circleByDiameter_;
}

QAction* UI::CircleMenu::circleByTwoPoints() const {
    return circleByTwoPoints_;
}

QAction* UI::CircleMenu::circleByThreePoints() const {
    return circleByThreePoints_;
}

QAction* UI::CircleMenu::circleTangentTwoLines() const {
    return circleTangentTwoLines_;
}

QAction* UI::CircleMenu::circleTangentThreeLines() const {
    return circleTangentThreeLines_;
}

