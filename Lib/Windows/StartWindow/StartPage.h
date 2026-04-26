#ifndef OURPAINT_DESKTOP_UI_STARTPAGE_H
#define OURPAINT_DESKTOP_UI_STARTPAGE_H

#include "../BasePage.h"

class QHBoxLayout;
class QVBoxLayout;

namespace UI {
    class TopBarStart;
    class AnimationArea;
    class ProjectsListWidget;


    // Landing page with recent projects.
    class StartPage final : public BasePage {
        Q_OBJECT

    public:
        explicit StartPage(QWidget *parent = nullptr);
        void setProjectsList(const QVector<QPair<QString, QString> >& projects) const;
        void setDefaultProjectsPath(const QString& projectPath);

    public slots:
        void onOpenProjectSlot();
        void onCreateProjectSlot();
    signals:
        void openProjectInThisWindowTriggered(const QString& projectPath);
        void createProjectInThisWindowTriggered(const QString& projectPath);
        void renameProjectTriggered(const QString& newName, const QString& path);
        void deleteProjectTriggered(const QString& path);

    protected:
        void changeEvent(QEvent *e) override;

    private:
        void initUI();
        void translate() const;

        TopBarStart *topBar_{nullptr};
        ProjectsListWidget *projectsListWidget_{nullptr};
        AnimationArea *animationArea_{nullptr};

        QHBoxLayout *contentLayout_{nullptr};

        QString defaultProjectsPath_;
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_STARTPAGE_H
