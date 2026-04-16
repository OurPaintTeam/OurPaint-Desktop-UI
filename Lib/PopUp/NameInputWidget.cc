#include "NameInputWidget.h"


UI::NameInputWidget::NameInputWidget(const QString& promptText, QWidget* parent)
    : InputWidget(promptText, parent) {
    setObjectName("NameInputWidget");
}
