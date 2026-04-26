#ifndef OURPAINT_DESKTOP_UI_PROJECTBUTTON_H
#define OURPAINT_DESKTOP_UI_PROJECTBUTTON_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>

namespace UI {

    class RenameTabLineEdit;

    class ProjectButton final : public QPushButton
    {
        Q_OBJECT

    public:
        explicit ProjectButton(const QString& name,
                               const QString& path,
                               QWidget* parent = nullptr);

        bool eventFilter(QObject *obj, QEvent *event) override;

        void startRenameInline();
        void finishRenameInline();
        void cancelRenameInline();

        QString getName() const;
        QString getPath() const;



    signals:
            void renameProjectTriggered(const QString& newName, const QString& path);
        void deleteProjectTriggered(const QString& path);
        void pressProjectTriggered(const QString& path);
    protected:
        void changeEvent(QEvent *e) override;

    private:
        void translate() const;

        QLabel* nameLabel_{nullptr};
        QLabel* pathLabel_{nullptr};

        RenameTabLineEdit* edit_{nullptr};
        QPushButton* renameBtn_{nullptr};
        QPushButton* deleteBtn_{nullptr};

        QVBoxLayout* layout_{nullptr};
        QHBoxLayout* topLayout_{nullptr};

        QWidget* nameContainer_{nullptr};
        QStackedLayout* nameStack_{nullptr};

        bool editing_ = false;
    };

} // namespace UI

#endif