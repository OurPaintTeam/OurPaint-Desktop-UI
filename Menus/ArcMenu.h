#ifndef OURPAINT_DESKTOP_UI_ARCMENU_H
#define OURPAINT_DESKTOP_UI_ARCMENU_H


#include "CustomMenu.h"

namespace UI {
    class ArcMenu final : public CustomMenu {
        Q_OBJECT

    public:
        explicit ArcMenu(QWidget* parent = nullptr);

    private:
        QAction* arcCenter_{nullptr};
        QAction* arc3Points_{nullptr};
        QAction* arcRadius_{nullptr};
        QAction* arcSettings_{nullptr};
    };
} // namespace UI


#endif // OURPAINT_DESKTOP_UI_ARCMENU_H
