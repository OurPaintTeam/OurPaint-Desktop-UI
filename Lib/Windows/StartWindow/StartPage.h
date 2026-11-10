#ifndef OURPAINT_DESKTOP_UI_STARTPAGE_H
#define OURPAINT_DESKTOP_UI_STARTPAGE_H

#include <QList>

#include "Utils.h"
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
        void setFilesList(const QVector<ProjectData>& files) const;
        void setDefaultFilesPath(const QString& path);

    public slots:
        void onOpenFileSlot();
        void onCreateFileSlot();
    signals:
        void openFileByIDTriggered(qint64 id, bool newWindow);
        void openFileTriggered(const QString& path, bool newWindow);
        void createFileTriggered(const QString& path, bool newWindow);
        void renameFileTriggered(FileData& data);
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
