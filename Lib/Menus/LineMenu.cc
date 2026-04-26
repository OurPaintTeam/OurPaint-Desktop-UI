#include "LineMenu.h"

#include "ToolsType.h"

#include <QEvent>

UI::LineMenu::LineMenu(QWidget* parent)
    : UI::CustomMenu(parent) {
    line_ = addAction(QIcon(":/Assets/icons/primitives/line.svg"), "");
    line_->setData(QVariant::fromValue(PrimitiveType::Line));

    polyLine_ = addAction(QIcon(":/Assets/icons/primitives/polyline.svg"), "");
    polyLine_->setData(QVariant::fromValue(PrimitiveType::Polyline));

    infiniteLine_ = addAction(QIcon(":/Assets/icons/primitives/infiniteLine.svg"), "");
    infiniteLine_->setData(QVariant::fromValue(PrimitiveType::InfiniteLine));

    addSeparator();
    settingsLine_ = addAction(QIcon(":/Assets/icons/tools/settings.svg"), "");
    settingsLine_->setData(QVariant::fromValue(PrimitiveType::LineSettings));

    for (auto* a: actions()) {
        if (!a->isSeparator()) {
            connect(a, &QAction::triggered, this, [this, a]() { emit actionSelected(a); });
        }
    }

    setObjectName(QStringLiteral("LineMenu"));
    setAttribute(Qt::WA_StyledBackground, true);

    translate();
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


void UI::LineMenu::translate() const {
    if (line_) {
        line_-> setText(UI::LineMenu::tr("Line"));
    }
    if (infiniteLine_) {
        infiniteLine_ -> setText(UI::LineMenu::tr("Infinite line"));
    }
    if (settingsLine_) {
        settingsLine_ -> setText(UI::LineMenu::tr("Line settings..."));
    }
    if (polyLine_) {
        polyLine_ -> setText(UI::LineMenu::tr("Polyline"));
    }
}