#ifndef OURPAINT_DESKTOP_UI_PROJECTSLISTWIDGET_H
#define OURPAINT_DESKTOP_UI_PROJECTSLISTWIDGET_H

#include <QHBoxLayout>
#include <QWidget>

namespace UI {
    class ProjectsListWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit ProjectsListWidget(QWidget* parent = nullptr);

    private:
        QHBoxLayout* layout_{nullptr};
    };
} // namespace UI
#endif //OURPAINT_DESKTOP_UI_PROJECTSLISTWIDGET_H
