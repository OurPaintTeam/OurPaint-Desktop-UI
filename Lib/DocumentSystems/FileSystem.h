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

    QString rootPath() const;
    QString projectsPath() const;
    QString settingsPath() const;
    QString scriptsPath() const;
    QString databasePath() const;

    bool isInitialized() const;
    bool initialize();

    qint64 defaultPathId() const;
    FsResult getPath(qint64 pathId, PathData& outPath) const;

    QVector<ProjectData> projects() ;
    FsResult getProject(qint64 projectId, ProjectData& outProject) ;
    FsResult getProjectPath(qint64 projectId, QString& outPath) ;
    FsResult getProjectIdByPath(const QString& fullPath, qint64& outId) const;

    FsResult createProject(const QString& name, qint64& outId);
    FsResult registerProject(const QString& fullPath, qint64& outId);
    FsResult renameProject(qint64 projectId, const QString& newName);
    FsResult removeProject(qint64 projectId);

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
    FsResult insertProject(const QString& baseName, qint64& outId);
    FsResult insertProjectAtPath(qint64 pathId, const QString& baseName, qint64& outId);
    FsResult findOrCreatePathEntry(const QString& directory, qint64& outPathId);
    FsResult getProjectInternal(qint64 projectId, ProjectData& outProject) ;
    FsResult validateProjectName(const QString& name, QString& outBaseName) const;
    FsResult migrateLegacyNames();

    static QString normalizePath(const QString& path);
    static QString pathKey(const QString& path);
    static bool isOurPaintFile(const QString& path);
    QString fullPathFor(const QString& directory, const QString& baseName);
};

} // namespace UI

#endif