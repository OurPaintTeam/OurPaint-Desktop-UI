#ifndef OURPAINT_DESKTOP_UI_NAMEINPUTWIDGET_H
#define OURPAINT_DESKTOP_UI_NAMEINPUTWIDGET_H
#include "InputWidget.h"


namespace UI {
    class NameInputWidget final : public InputWidget {
        Q_OBJECT

    public:
        explicit NameInputWidget(const QString& promptText, QWidget* parent = nullptr);
    };
} // namespace UI


#endif //OURPAINT_DESKTOP_UI_NAMEINPUTWIDGET_H
