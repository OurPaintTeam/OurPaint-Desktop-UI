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
        void setLeftClick(bool acceptLeftClick);
        void setRightClick(bool acceptRightClick);

    protected:
        void paintEvent(QPaintEvent* event) override;
        void mousePressEvent(QMouseEvent* e) override;

    private:
        QMenu* menu_{nullptr};
        bool draw_{true};
        bool acceptLeftClick_{true};
        bool acceptRightClick_{true};

    private slots:
        void onArcActionSelected(const QAction* action);
    };
}// namespace UI


#endif //OURPAINT_DESKTOP_UI_MENUBUTTON_H
