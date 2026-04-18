#include "CircleMenu.h"

#include "ToolsType.h"


UI::CircleMenu::CircleMenu(QWidget* parent)
    : UI::CustomMenu(parent) {
    circleByDiameter_ = addAction(QIcon(":/Assets/icons/primitives/circleByDiameter.png"), "Circle by diameter");
    circleByDiameter_->setData(QVariant::fromValue(PrimitiveType::CircleByDiameter));

    circleByRadius_ = addAction(QIcon(":/Assets/icons/primitives/circleByRadius.svg"), "Circle by radius");
    circleByRadius_->setData(QVariant::fromValue(PrimitiveType::CircleByRadius));

    ellipse_ = addAction(QIcon(":/Assets/icons/primitives/ellipseThreePoints.svg"), "Ellipse by 3 points");
    ellipse_->setData(QVariant::fromValue(PrimitiveType::EllipseThreePoints));

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


QAction* UI::CircleMenu::circleByDiameter() const {
  return circleByDiameter_;
}

QAction* UI::CircleMenu::circleByRadius() const {
  return circleByRadius_;
}

QAction* UI::CircleMenu::ellipse() const {
  return ellipse_;
}

QAction* UI::CircleMenu::circleSettings() const {
  return circleSettings_;
}