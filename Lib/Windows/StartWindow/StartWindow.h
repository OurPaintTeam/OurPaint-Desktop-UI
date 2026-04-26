#ifndef OURPAINT_DESKTOP_UI_STARTWINDOW_H
#define OURPAINT_DESKTOP_UI_STARTWINDOW_H

#include "BaseWindow.h"

class QVBoxLayout;

namespace UI {
    class StartPage;


    class StartWindow : public BaseWindow {
        Q_OBJECT

    public:
        explicit StartWindow(QWidget* parent = nullptr);
        ~StartWindow() override = default;

        void initUI() override;
        void setProjectsList(const QVector<QPair<QString, QString>>& projectsList) const;
        void setDefaultProjectsPath(const QString& projectPath) const;

    signals:
        void openProjectThisWindowTriggered(const QString& projectPath);
        void createProjectThisWindowTriggered(const QString& projectPath);
        void renameProjectTriggered(const QString& newName, const QString& path);
        void deleteProjectTriggered(const QString& path);

    private:
        QWidget* contentWidget_{nullptr};
        QVBoxLayout* rootLayout_{nullptr};
        StartPage* startPage_{nullptr};
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_STARTWINDOW_H
