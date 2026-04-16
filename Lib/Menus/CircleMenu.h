#ifndef OURPAINT_DESKTOP_UI_CIRCLEMENU_H
#define OURPAINT_DESKTOP_UI_CIRCLEMENU_H

#include "CustomMenu.h"

namespace UI {
    class CircleMenu final : public CustomMenu
    {
        Q_OBJECT
    public:
        explicit CircleMenu(QWidget* parent = nullptr);
        private:
        QAction* circleCenter_{nullptr};
        QAction* circleTwoPoints_{nullptr};
        QAction* circleThreePoints_{nullptr};
        QAction* circleSettings_{nullptr};
    };
}// namespace UI

#endif // OURPAINT_DESKTOP_UI_CIRCLEMENU_H
