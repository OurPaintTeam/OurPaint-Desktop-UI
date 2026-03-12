#ifndef OURPAINT_DESKTOP_UI_VERSIONCONTROLMENU_H
#define OURPAINT_DESKTOP_UI_VERSIONCONTROLMENU_H

#include "CustomMenu.h"

namespace UI {
    class VersionControlMenu final : public CustomMenu {
        Q_OBJECT

    public:
        explicit VersionControlMenu(QWidget* parent = nullptr);

    private:
        QAction* init_{nullptr};
        QAction* create_{nullptr};
        QAction* push_{nullptr};
        QAction* pull_{nullptr};
        QAction* commit_{nullptr};
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_VERSIONCONTROLMENU_H
