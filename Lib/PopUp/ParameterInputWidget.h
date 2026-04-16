#ifndef OURPAINT_DESKTOP_UI_PARAMETERINPUTWIDGET_H
#define OURPAINT_DESKTOP_UI_PARAMETERINPUTWIDGET_H
#include "InputWidget.h"


namespace UI {
    class ParameterInputWidget final : public InputWidget {
        Q_OBJECT

    public:
        explicit ParameterInputWidget(const QString& promptText, QWidget* parent = nullptr);
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_PARAMETERINPUTWIDGET_H
