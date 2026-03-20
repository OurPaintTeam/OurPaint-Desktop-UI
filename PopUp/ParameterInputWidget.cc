#include "ParameterInputWidget.h"


UI::ParameterInputWidget::ParameterInputWidget(const QString& promptText, QWidget* parent)
    : InputWidget(promptText, parent) {
    setObjectName("ParameterInputWidget");
}
