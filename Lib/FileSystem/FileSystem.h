#ifndef OURPAINT_DESKTOP_UI_FILESYSTEM_H
#define OURPAINT_DESKTOP_UI_FILESYSTEM_H
#include <QList>
#include <QString>

namespace UI {
    class FileSystem {
    public:
        struct ProjectData {
            QString id{};
            QString name{};
            QString path{};
            QVector<QString> tabs{};
        };


        struct FileData {
            QString tabName{};
            QString projectId{};
            QString projectPath{};
        };


        enum class FsResult {
            Ok,
            AlreadyExists,
            NotFound,
            IoError
        };


        FileSystem();
        QString defaultRoot() const;
        QString defaultProjectsPath() const;
        QVector<QPair<QString, QString> > projects() const;
        FsResult getProjectDataById(const QString& projectId, ProjectData& outData) const;
        FsResult getProjectIdByPath(const QString& fullPath, QString& outProjectId) const;
        FsResult createProject(const QString& fullPath, QString& outProjectId) const;
        FsResult openProjectById(const QString& projectId, ProjectData& outData) const;
        FsResult openProjectByPath(const QString& fullPath, ProjectData& outData) const;
        FsResult openFile(const QString& filePath, FileData& outData) const;
        FsResult removeProjectById(const QString& projectId) const;
        FsResult renameProjectById(const QString& projectId, const QString& newName, ProjectData& outData) const;
        FsResult renameProjectByPath(const QString& fullPath, const QString& newName, ProjectData& outData) const;
        FsResult createTab(const QString& projectId, const QString& tabName) const;
        FsResult removeTab(const QString& projectId, const QString& tabName) const;
        FsResult renameTab(const QString& projectId, const QString& oldName, const QString& newName) const;

    private:
        QString rootPath_;
        QString projectsDir_;
        QString settingsDir_;
        QString projectsXml_;

        QVector<ProjectData> loadProjects() const;

        FsResult addProject(const QString& name,
                            const QString& path,
                            const QString& outId) const;

        FsResult removeProjectFromXml(const QString& projectId) const;

        FsResult renameProjectInXml(const QString& projectId,
                                    const QString& newPath,
                                    const QString& newName) const;

        void initialize() const;

        static QString extractProjectNameFromPath(const QString& fullPath);
        static QString generateProjectId();
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_FILESYSTEM_H
