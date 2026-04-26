#ifndef OURPAINT_DESKTOP_UI_PROJECTSLISTWIDGET_H
#define OURPAINT_DESKTOP_UI_PROJECTSLISTWIDGET_H

#include <QWidget>

class QVBoxLayout;
class QHBoxLayout;
class QLineEdit;
class QPushButton;
class QScrollArea;
class QFrame;


namespace UI {
    class ProjectButton;


    class ProjectsListWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit ProjectsListWidget(QWidget *parent = nullptr);
        void setProjects(const QVector<QPair<QString, QString> >& projects);

    signals:
        void renameProjectTriggered(const QString& newName, const QString& path);
        void deleteProjectTriggered(const QString& path);
        void pressProjectTriggered(const QString& path);

        void pressCreateButtonTriggered();
        void pressOpenButtonTriggered();

        protected:
        void changeEvent(QEvent *e) override;
    private slots:
        void filterProjectsSlot(const QString& text);

    private:
        void translate() const;
        // Private variables with namePtr_ style
        QVBoxLayout *rootLayoutPtr_{nullptr};
        QWidget *openPanelPtr_{nullptr};
        QVBoxLayout *panelLayoutPtr_{nullptr};
        QWidget *inputContainerPtr_{nullptr};
        QHBoxLayout *inputLayoutPtr_{nullptr};
        QLineEdit *projectInput_{nullptr};
        QPushButton *createButtonPtr_{nullptr};
        QPushButton *openButtonPtr_{nullptr};
        QFrame *linePtr_{nullptr};
        QScrollArea *scrollAreaPtr_{nullptr};
        QWidget *scrollContent_{nullptr};
        QVBoxLayout *projectsLayout_{nullptr};
        QList<ProjectButton*> allProjects_{};

        // Constants
        static constexpr int WIDGET_WIDTH = 300;
        static constexpr int PANEL_MARGIN = 15;
        static constexpr int INPUT_MAX_HEIGHT = 30;
        static constexpr int BUTTON_SIZE = 30;
        static constexpr int CONTENT_MARGINS_LEFT = 4;
        static constexpr int CONTENT_MARGINS_TOP = 4;
        static constexpr int CONTENT_MARGINS_RIGHT = 8;
        static constexpr int CONTENT_MARGINS_BOTTOM = 4;
        static constexpr int LAYOUT_SPACING = 2;
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_PROJECTSLISTWIDGET_H
