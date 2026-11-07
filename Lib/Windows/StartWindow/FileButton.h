#ifndef OURPAINT_DESKTOP_UI_PROJECTBUTTON_H
#define OURPAINT_DESKTOP_UI_PROJECTBUTTON_H

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>

namespace UI {
    struct FileData;

    class RenameTabLineEdit;

    class FileButton final : public QPushButton
    {
        Q_OBJECT

    public:
        explicit FileButton(
        const FileData& file,
        QWidget* parent = nullptr
    );

        bool eventFilter(QObject *obj, QEvent *event) override;

        void startRenameInline();
        void finishRenameInline();
        void cancelRenameInline();

        QString getName() const;
        QString getPath() const;

    signals:
        void renameFileTriggered(qint64 fileId, const QString& newName);
        void deleteFileTriggered(qint64 fileId);
        void pressFileTriggered(qint64 fileId);
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

        qint64 fileId_{-1};
    };

} // namespace UI

#endif