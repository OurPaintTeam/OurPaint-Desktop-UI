#ifndef OURPAINT_DESKTOP_UI_CIRCLEMENU_H
#define OURPAINT_DESKTOP_UI_CIRCLEMENU_H

#include "CustomMenu.h"

namespace UI {
    class CircleMenu final : public CustomMenu {
        Q_OBJECT

    public:
        explicit CircleMenu(QWidget* parent = nullptr);
        QAction* circleByRadius() const;
        QAction* circleByDiameter() const;
        QAction* circleByTwoPoints() const;
        QAction* circleByThreePoints() const;
        QAction* circleTangentTwoLines() const;
        QAction* circleTangentThreeLines() const;



    private:
        QAction* circleByRadius_{nullptr};
        QAction* circleByDiameter_{nullptr};
        QAction* circleByTwoPoints_{nullptr};
        QAction* circleByThreePoints_{nullptr};
        QAction* circleTangentTwoLines_{nullptr};
        QAction* circleTangentThreeLines_{nullptr};
    };
}// namespace UI

#endif // OURPAINT_DESKTOP_UI_CIRCLEMENU_H
