#ifndef OURPAINT_DESKTOP_UI_STARTWINDOW_H
#define OURPAINT_DESKTOP_UI_STARTWINDOW_H

#include <QList>

#include "BaseWindow.h"
#include "Utils.h"

class QVBoxLayout;

namespace UI {
    struct FileData;
    class StartPage;


    class StartWindow : public BaseWindow {
        Q_OBJECT

    public:
        explicit StartWindow(QWidget* parent = nullptr);
        ~StartWindow() override = default;

        void initUI() override;
        void setFilesList(const QVector<ProjectData>& files) const;
        void setDefaultFilesPath(const QString& path) const;

    signals:
        void openFileByIDTriggered(qint64 id, bool newWindow);
        void openFileTriggered(const QString& path, bool newWindow);
        void createFileTriggered(const QString& path, bool newWindow);
        void renameFileTriggered(FileData& data);
        void deleteFileTriggered(const qint64 id);

    private:
        QWidget* contentWidget_{nullptr};
        QVBoxLayout* rootLayout_{nullptr};
        StartPage* startPage_{nullptr};
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_STARTWINDOW_H
