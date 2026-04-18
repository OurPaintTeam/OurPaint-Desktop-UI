#ifndef OURPAINT_DESKTOP_UI_CIRCLEMENU_H
#define OURPAINT_DESKTOP_UI_CIRCLEMENU_H

#include "CustomMenu.h"

namespace UI {
    class CircleMenu final : public CustomMenu
    {
        Q_OBJECT
    public:
        explicit CircleMenu(QWidget* parent = nullptr);
      QAction* circleByDiameter() const;
      QAction* circleByRadius() const;
      QAction* ellipse() const;
      QAction* circleSettings() const;
        private:
        QAction* circleByDiameter_{nullptr};
        QAction* circleByRadius_{nullptr};
        QAction* ellipse_{nullptr};
        QAction* circleSettings_{nullptr};
    };
}// namespace UI

#endif // OURPAINT_DESKTOP_UI_CIRCLEMENU_H
