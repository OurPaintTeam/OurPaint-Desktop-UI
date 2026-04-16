#ifndef OURPAINT_DESKTOP_UI_PROJECTBUTTON_H
#define OURPAINT_DESKTOP_UI_PROJECTBUTTON_H


#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace UI {
    class ProjectButton final : public QPushButton
    {
        Q_OBJECT

    public:
        explicit ProjectButton(const QString& name,
                               const QString& path,
                               QWidget* parent = nullptr);
        QString getName() const;
        QString getPath() const;
    signals:
        void renameProjectTriggered(const QString& newName, const QString& path);
        void deleteProjectTriggered(const QString& path);
        void pressProjectTriggered(const QString& path);

    private:
        QLabel* nameLabel_{nullptr};
        QLabel* pathLabel_{nullptr};

        QPushButton* renameBtn_{nullptr};
        QPushButton* deleteBtn_{nullptr};

        QVBoxLayout* layout_{nullptr};
        QHBoxLayout* topLayout_{nullptr};
    };
}// namespace UI


#endif //OURPAINT_DESKTOP_UI_PROJECTBUTTON_H