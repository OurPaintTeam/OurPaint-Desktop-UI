#ifndef OURPAINT_DESKTOP_UI_LINEMENU_H
#define OURPAINT_DESKTOP_UI_LINEMENU_H

#include "CustomMenu.h"

namespace UI {
    class LineMenu final : public CustomMenu {
        Q_OBJECT

    public:
        explicit LineMenu(QWidget* parent = nullptr);

    private:
        QAction* line_{nullptr};
        QAction* polyLine_{nullptr};
        QAction* infiniteLine_{nullptr};
        QAction* settingsLine_{nullptr};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_LINEMENU_H
