#ifndef OURPAINT_DESKTOP_UI_NAVIGATIONWIDGET_H
#define OURPAINT_DESKTOP_UI_NAVIGATIONWIDGET_H

#include <QWidget>

class QPushButton;
class QVBoxLayout;

namespace UI {
    class NavigationWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit NavigationWidget(QWidget* parent = nullptr);

    signals:
        void goToStartWindowTriggered();
        void createFileTriggered();
        void createProjectTriggered();
        void closeApplicationTriggered();

    private:
        QPushButton* startButton_{nullptr};
        QPushButton* createFileButton_{nullptr};
        QPushButton* createProjectButton_{nullptr};
        QPushButton* closeButton_{nullptr};

        QVBoxLayout* mainLayout_{nullptr};
        QWidget* buttonPlane_ {nullptr};
        QVBoxLayout* layout_ {nullptr};
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_NAVIGATIONWIDGET_H
