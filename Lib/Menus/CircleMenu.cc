#include "CircleMenu.h"

#include "ToolsType.h"

#include <QEvent>


UI::CircleMenu::CircleMenu(QWidget *parent)
    : UI::CustomMenu(parent) {
    // Radius
    circleByRadius_ = addAction(QIcon(":/Assets/icons/primitives/circleByRadius.svg"), "");
    circleByRadius_->setData(QVariant::fromValue(PrimitiveType::CircleByRadius));

    // Diameter
    circleByDiameter_ = addAction(QIcon(":/Assets/icons/primitives/circleByDiameter.svg"), "");
    circleByDiameter_->setData(QVariant::fromValue(PrimitiveType::CircleByDiameter));

    // Two points
    circleByTwoPoints_ = addAction(QIcon(":/Assets/icons/primitives/circleByDiameter.svg"), "");
    circleByTwoPoints_->setData(QVariant::fromValue(PrimitiveType::CircleByTwoPoints));

    // Three points
    circleByThreePoints_ = addAction(QIcon(":/Assets/icons/primitives/circleByDiameter.svg"), "");
    circleByThreePoints_->setData(QVariant::fromValue(PrimitiveType::CircleByThreePoints));

    // Tangent two lines
    circleTangentTwoLines_ = addAction(QIcon(":/Assets/icons/primitives/circleByDiameter.svg"), "");
    circleTangentTwoLines_->setData(QVariant::fromValue(PrimitiveType::CircleTangentTwoLines));

    // Tangent three lines
    circleTangentThreeLines_ = addAction(QIcon(":/Assets/icons/primitives/circleByDiameter.svg"), "");
    circleTangentThreeLines_->setData(QVariant::fromValue(PrimitiveType::CircleTangentThreeLines));


    addSeparator();
    // circleSettings_ = addAction(QIcon(":/Assets/icons/tools/settings.svg"), "Circle settings...");
    // circleSettings_->setData(QVariant::fromValue(PrimitiveType::CircleSettings));

    for (auto *a: actions()) {
        if (!a->isSeparator()) {
            connect(a, &QAction::triggered, this, [this, a]() {
                emit actionSelected(a);
            });
        }
    }

    setObjectName(QStringLiteral("CircleMenu"));
    setAttribute(Qt::WA_StyledBackground, true);

    translate();
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


void UI::CircleMenu::translate() const {
    if (circleByRadius_) {
        // Radius
        circleByRadius_->setText(UI::CircleMenu::tr("Circle by radius"));
    }
    if (circleByDiameter_) { // Diameter
        circleByDiameter_->setText(UI::CircleMenu::tr("Circle by diameter"));
    }
    if (circleByTwoPoints_) { // Two points
        circleByTwoPoints_->setText(UI::CircleMenu::tr("Circle by two points"));
    }
    if (circleByThreePoints_) {
        // Three points
        circleByThreePoints_->setText(UI::CircleMenu::tr("Circle by three points"));
    }
    if (circleTangentTwoLines_) {
        // Tangent two lines
        circleTangentTwoLines_->setText(UI::CircleMenu::tr("Circle tangent two lines"));
    }
    if (circleTangentThreeLines_) {
        // Tangent three lines
        circleTangentThreeLines_->setText(UI::CircleMenu::tr("Circle tangent three lines"));
    }
}
