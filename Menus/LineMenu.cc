#include "LineMenu.h"


UI::LineMenu::LineMenu(QWidget* parent)
    : UI::CustomMenu(parent) {
    line_ = addAction("Line");
    polyLine_ = addAction("Polyline");
    infiniteLine_ = addAction("Infinite line");
    addSeparator();
    settingsLine_ = addAction("Line settings...");

    setObjectName(QStringLiteral("LineMenu"));
}
