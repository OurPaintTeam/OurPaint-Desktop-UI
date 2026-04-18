#ifndef OURPAINT_DESKTOP_UI_MENUBUTTON_H
#define OURPAINT_DESKTOP_UI_MENUBUTTON_H
#include <QPushButton>


namespace UI {
    class MenuButton final : public QPushButton {
        Q_OBJECT

    public:
        explicit MenuButton(const QString& text, QMenu* menu, QWidget* parent = nullptr);

        void setInitialFromMenu();
        void bindMenu(QMenu* menu);
        void setDraw(bool draw);
        void setOpenLeftClick(bool acceptLeftClick);
        void setChange(bool acceptChange);
        void applyAction(const QAction* action);
    signals:
        void leftClickedTriggered(const QAction* action);

    protected:
        void paintEvent(QPaintEvent* event) override;
        void mousePressEvent(QMouseEvent* e) override;
        void wheelEvent(QWheelEvent* event) override;

    private:
        QMenu* menu_{nullptr};
        bool draw_{true};
        bool openLeftClick_{true};
        bool acceptChange_{false};
        const QAction* activeAction_{nullptr};
    };
} // namespace UI


#endif //OURPAINT_DESKTOP_UI_MENUBUTTON_H
