#ifndef OURPAINT_DESKTOP_UI_FILESYSTEM_H
#define OURPAINT_DESKTOP_UI_FILESYSTEM_H

#include <QString>
#include <QSqlDatabase>
#include <QVector>

namespace UI {

class FileSystem {
public:
    struct PathData {
        qint64 id = -1;
        QString path;
    };

    struct ProjectData {
        qint64 id = -1;
        QString name;
        QString path;
        qint64 pathId = -1;
    };

    enum class FsResult {
        Ok,
        AlreadyExists,
        NotFound,
        IoError,
        InvalidArgument
    };

    FileSystem();
    ~FileSystem();

    FileSystem(const FileSystem&) = delete;
    FileSystem& operator=(const FileSystem&) = delete;

    // Directories are platform-independent and based on DocumentsLocation.
    QString rootPath() const;
    QString projectsPath() const;
    QString settingsPath() const;
    QString scriptsPath() const;
    QString databasePath() const;

    bool isInitialized() const;
    bool initialize();

    qint64 defaultPathId() const;
    FsResult getPath(qint64 pathId, PathData& outPath) const;

    // Every listing/read operation reconciles the database with the disk first.
    QVector<ProjectData> projects() const;
    FsResult getProject(qint64 projectId, ProjectData& outProject) const;
    FsResult getProjectPath(qint64 projectId, QString& outPath) const;
    FsResult getProjectIdByPath(const QString& fullPath, qint64& outId) const;

    // Projects are stored directly in OurPaint/Projects.
    FsResult createProject(const QString& name, qint64& outId);
    FsResult registerProject(const QString& fullPath, qint64& outId);
    FsResult renameProject(qint64 projectId, const QString& newName);
    FsResult moveProject(qint64 projectId, qint64 newPathId);
    FsResult removeProject(qint64 projectId);

    // Removes stale records and discovers .ourp files added outside the application.
    FsResult synchronize();

private:
    QString rootPath_;
    QString projectsDir_;
    QString settingsDir_;
    QString scriptsDir_;
    QString databasePath_;
    QString connectionName_;

    QSqlDatabase database_;
    bool initialized_ = false;

    FsResult synchronizeInternal();
    FsResult insertProject(const QString& fileName, qint64& outId);
    FsResult getProjectInternal(qint64 projectId, ProjectData& outProject) const;
    FsResult validateProjectName(const QString& name, QString& outName) const;

    static QString normalizePath(const QString& path);
    static QString pathKey(const QString& path);
    static bool isOurPaintFile(const QString& path);
};

} // namespace UI

#endif // OURPAINT_DESKTOP_UI_FILESYSTEM_H
