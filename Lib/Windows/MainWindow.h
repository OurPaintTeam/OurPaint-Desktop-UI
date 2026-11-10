#ifndef OURPAINT_DESKTOP_UI_MAINWINDOW_H
#define OURPAINT_DESKTOP_UI_MAINWINDOW_H

#include "FramelessWindow.h"
#include <QStackedWidget>

namespace UI {
    class StartPage;
    class EditorPage;
    class SettingsSystem;
    class NotificationContainer;

    class MainWindow final : public FramelessWindow {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent = nullptr);
        ~MainWindow() override = default;

        void initUI() override;

        QStackedWidget* stack() const { return stack_; }
        StartPage* startPage() const { return startPage_; }
        EditorPage* editorPage() const { return editorPage_; }

        void addNotification(const QString& text) const;

    private:
        QStackedWidget* stack_{nullptr};
        StartPage* startPage_{nullptr};
        EditorPage* editorPage_{nullptr};
        NotificationContainer* notificationManager_{nullptr};
    };
}
#endif