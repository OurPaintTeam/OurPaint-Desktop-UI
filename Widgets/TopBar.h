#ifndef OURPAINT_DESKTOP_UI_TOPBAR_H
#define OURPAINT_DESKTOP_UI_TOPBAR_H
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>


namespace UI {
    class HideOpenPanelButton;


    // Common window title bar with system buttons.
    class TopBar : public QWidget {
        Q_OBJECT

    public:
        explicit TopBar(QWidget* parent = nullptr);

    protected:
        QPushButton* createWindowButton();
        HideOpenPanelButton* createPanelButton();
        [[nodiscard]] QHBoxLayout* getLayout() const;

    private:
        QPushButton* minButton_{nullptr};
        QPushButton* maxButton_{nullptr};
        QPushButton* closeButton_{nullptr};
        QHBoxLayout* layout_{nullptr};

        void setupStyle();
        void setupWindowButtons();
    };
} // namespace UI
#endif // OURPAINT_DESKTOP_UI_TOPBAR_H
