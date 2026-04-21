#include "ArcMenu.h"

#include <QAction>

#include "ToolsType.h"


UI::ArcMenu::ArcMenu(QWidget* parent)
    : UI::CustomMenu(parent) {
    arcCenter_ = addAction(QIcon(":/Assets/icons/primitives/arcByRadius.svg"), "Arc by radius");
    arcCenter_->setData(QVariant::fromValue(PrimitiveType::ArcByRadius));

    arc3Points_ = addAction(QIcon(":/Assets/icons/primitives/arcDiameter.svg"), "Arc by diameter");
    arc3Points_->setData(QVariant::fromValue(PrimitiveType::ArcByDiameter));

    arcRadius_ = addAction(QIcon(":/Assets/icons/primitives/arc3points.svg"), "Arc by 3 point");
    arcRadius_->setData(QVariant::fromValue(PrimitiveType::ArcThreePoints));

    addSeparator();
    arcSettings_ = addAction(QIcon(":/Assets/icons/tools/settings.svg"), "Arc settings...");
    arcSettings_->setData(QVariant::fromValue(PrimitiveType::ArcSettings));

    for (auto* a: actions()) {
        if (!a->isSeparator()) {
            connect(a, &QAction::triggered, this, [this, a]() {
                emit actionSelected(a);
            });
        }
    }

    setObjectName(QStringLiteral("ArcMenu"));
    setAttribute(Qt::WA_StyledBackground, true);
}


QAction* UI::ArcMenu::arcCenter() const {
  return arcCenter_;
}

QAction* UI::ArcMenu::arc3Points() const {
  return arc3Points_;
}

QAction* UI::ArcMenu::arcRadius() const {
  return arcRadius_;
}

QAction* UI::ArcMenu::arcSettings() const {
  return arcSettings_;
}