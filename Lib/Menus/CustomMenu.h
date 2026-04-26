#ifndef OURPAINT_DESKTOP_UI_CUSTOMMENU_H
#define OURPAINT_DESKTOP_UI_CUSTOMMENU_H

#include <QMenu>
class QShowEvent;

namespace UI {
    class CustomMenu : public QMenu {
        Q_OBJECT

    protected:
        void showEvent(QShowEvent *event) override;

    public:
        explicit CustomMenu(QWidget *parent = nullptr);

    signals:
        void actionSelected(QAction *action);

    protected:
        void changeEvent(QEvent *e) override;

        virtual void translate() const = 0;

    private:
        void setupWindow();
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_CUSTOMMENU_H
