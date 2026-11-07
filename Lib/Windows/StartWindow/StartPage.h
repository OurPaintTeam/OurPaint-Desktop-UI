#ifndef OURPAINT_DESKTOP_UI_STARTPAGE_H
#define OURPAINT_DESKTOP_UI_STARTPAGE_H

#include "../BasePage.h"

class QHBoxLayout;
class QVBoxLayout;

namespace UI {
    struct FileData;
    class TopBarStart;
    class AnimationArea;
    class FilesListWidget;


    // Landing page with recent Files.
    class StartPage final : public BasePage {
        Q_OBJECT

    public:
        explicit StartPage(QWidget *parent = nullptr);
        void setFilesList(const QVector<FileData>& files) const;
        void setDefaultFilesPath(const QString& path);

    public slots:
        void onOpenFileSlot();
        void onCreateFileSlot();
    signals:
        void pressFileTriggered(const qint64 id);
        void openFileInThisWindowTriggered(const QString& path);
        void createFileInThisWindowTriggered(const QString& path);
        void renameFileTriggered(const qint64 id, const QString& path);
        void deleteFileTriggered(const qint64 id);

    protected:
        void changeEvent(QEvent *e) override;

    private:
        void initUI();
        void translate() const;

        TopBarStart *topBar_{nullptr};
        FilesListWidget *filesListWidget_{nullptr};
        AnimationArea *animationArea_{nullptr};

        QHBoxLayout *contentLayout_{nullptr};

        QString defaultFilesPath_;
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_STARTPAGE_H
