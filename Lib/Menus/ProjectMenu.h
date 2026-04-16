#ifndef OURPAINT_DESKTOP_UI_PROJECTMENU_H
#define OURPAINT_DESKTOP_UI_PROJECTMENU_H


#include "CustomMenu.h"

namespace UI {
    class ProjectMenu final : public CustomMenu {
        Q_OBJECT

    public:
        explicit ProjectMenu(QWidget* parent = nullptr);

    signals:
        void createProjectTriggered();
        void openProjectTriggered();
        void createFileTriggered();
        void openFileTriggered();
        void exportFileTriggered();
        void scriptTriggered();

    private:
        QAction* createProject_{nullptr};
        QAction* openProject_{nullptr};
        QAction* createFile_{nullptr};
        QAction* exportFile_{nullptr};
        QAction* openFile_{nullptr};
        QAction* script_{nullptr};
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_PROJECTMENU_H
