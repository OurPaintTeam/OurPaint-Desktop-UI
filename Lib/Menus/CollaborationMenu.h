#ifndef OURPAINT_DESKTOP_UI_COLLABORATIONMENU_H
#define OURPAINT_DESKTOP_UI_COLLABORATIONMENU_H

#include "CustomMenu.h"
#include "MenuTypes.h"


namespace UI {
    class CollaborationMenu final : public CustomMenu {
        Q_OBJECT

    public:
        explicit CollaborationMenu(QWidget *parent = nullptr);

    signals:
        void collabTriggered(UI::collab с);

    protected:
        void translate() const override;

    private:
        QAction *close_{nullptr};
        QAction *open_{nullptr};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_COLLABORATIONMENU_H
