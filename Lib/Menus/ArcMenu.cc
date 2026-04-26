#include "ArcMenu.h"

#include "ToolsType.h"

#include <QAction>
#include <QEvent>


UI::ArcMenu::ArcMenu(QWidget *parent)
    : UI::CustomMenu(parent) {
    arcCenter_ = addAction(QIcon(":/Assets/icons/primitives/arcByRadius.svg"), "");
    arcCenter_->setData(QVariant::fromValue(PrimitiveType::ArcByRadius));

    arc3Points_ = addAction(QIcon(":/Assets/icons/primitives/arcDiameter.svg"), "");
    arc3Points_->setData(QVariant::fromValue(PrimitiveType::ArcByDiameter));

    arcRadius_ = addAction(QIcon(":/Assets/icons/primitives/arc3points.svg"), "");
    arcRadius_->setData(QVariant::fromValue(PrimitiveType::ArcThreePoints));

    addSeparator();
    arcSettings_ = addAction(QIcon(":/Assets/icons/tools/settings.svg"), "");
    arcSettings_->setData(QVariant::fromValue(PrimitiveType::ArcSettings));

    for (auto *a: actions()) {
        if (!a->isSeparator()) {
            connect(a, &QAction::triggered, this, [this, a]() {
                emit actionSelected(a);
            });
        }
    }

    setObjectName(QStringLiteral("ArcMenu"));
    setAttribute(Qt::WA_StyledBackground, true);

    translate();
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


void UI::ArcMenu::translate() const {
    if (arcCenter_) {
        arcCenter_->setText(UI::ArcMenu::tr("Arc by radius"));
    }
    if (arc3Points_) {
        arc3Points_->setText(UI::ArcMenu::tr("Arc by diameter"));
    }
    if (arcRadius_) {
        arcRadius_->setText(UI::ArcMenu::tr("Arc by 3 point"));
    }
    if (arcSettings_) {
        arcSettings_->setText(UI::ArcMenu::tr("Arc settings..."));
    }
}
