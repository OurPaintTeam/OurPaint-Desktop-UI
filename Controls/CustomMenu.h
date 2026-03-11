#ifndef OURPAINT_DESKTOP_UI_CUSTOMMENU_H
#define OURPAINT_DESKTOP_UI_CUSTOMMENU_H

#include <QMenu>

namespace UI {
    class CustomMenu : public QMenu {
        Q_OBJECT

    public:
        explicit CustomMenu(QWidget* parent = nullptr);

    private:
        void setupWindow();
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_CUSTOMMENU_H
