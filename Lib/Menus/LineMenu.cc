#include "LineMenu.h"

#include "ToolsType.h"


UI::LineMenu::LineMenu(QWidget* parent)
    : UI::CustomMenu(parent) {
    line_ = addAction(QIcon(":/Assets/icons/primitives/line.svg"), "Line");
    line_->setData(QVariant::fromValue(PrimitiveType::Line));

    polyLine_ = addAction(QIcon(":/Assets/icons/primitives/polyline.svg"), "Polyline");
    polyLine_->setData(QVariant::fromValue(PrimitiveType::Polyline));

    infiniteLine_ = addAction(QIcon(":/Assets/icons/primitives/infiniteLine.svg"), "Infinite line");
    infiniteLine_->setData(QVariant::fromValue(PrimitiveType::InfiniteLine));

    addSeparator();
    settingsLine_ = addAction(QIcon(":/Assets/icons/tools/settings.svg"), "Line settings...");
    settingsLine_->setData(QVariant::fromValue(PrimitiveType::LineSettings));

    for (auto* a: actions()) {
        if (!a->isSeparator()) {
            connect(a, &QAction::triggered, this, [this, a]() { emit actionSelected(a); });
        }
    }

    setObjectName(QStringLiteral("LineMenu"));
    setAttribute(Qt::WA_StyledBackground, true);
}


QAction* UI::LineMenu::line() const {
  return line_;
}

QAction* UI::LineMenu::polyLine() const {
  return polyLine_;
}

QAction* UI::LineMenu::infiniteLine() const {
  return infiniteLine_;
}

QAction* UI::LineMenu::settingsLine() const {
  return settingsLine_;
}