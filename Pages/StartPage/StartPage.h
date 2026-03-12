#ifndef OURPAINT_DESKTOP_UI_STARTPAGE_H
#define OURPAINT_DESKTOP_UI_STARTPAGE_H

#include <QVBoxLayout>
#include <QWidget>

namespace UI {
    class TopBarStart;
    class AnimationArea;
    class ProjectsListWidget;


    // Landing page with recent projects.
    class StartPage final : public QWidget {
        Q_OBJECT

    public:
        explicit StartPage(QWidget* parent = nullptr);

    private:
        void initUI();

        TopBarStart* topBar_{nullptr};
        ProjectsListWidget* projectsListWidget_{nullptr};
        AnimationArea* animationArea_{nullptr};

        QVBoxLayout* mainLayout_{nullptr};
        QWidget* mainArea_{nullptr};
        QHBoxLayout* contentLayout_{nullptr};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_STARTPAGE_H
