#ifndef OURPAINT_DESKTOP_UI_PROJECTMENU_H
#define OURPAINT_DESKTOP_UI_PROJECTMENU_H


#include "CustomMenu.h"

namespace UI {
    class FileMenu final : public CustomMenu {
        Q_OBJECT

    public:
        explicit FileMenu(QWidget *parent = nullptr);

    signals:
        void createFileTriggered();
        void openFileTriggered();
        void exportFileTriggered();
        void scriptTriggered();

    protected:
        void translate() const override;

    private:
        QAction *createFile_{nullptr};
        QAction *exportFile_{nullptr};
        QAction *openFile_{nullptr};
        QAction *script_{nullptr};
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_PROJECTMENU_H
