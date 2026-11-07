#ifndef OURPAINT_DESKTOP_UI_FILESYSTEM_H
#define OURPAINT_DESKTOP_UI_FILESYSTEM_H

#include <QPair>
#include <QString>
#include <QSqlDatabase>
#include <QVector>

#include "Utils.h"

namespace UI {

class FileSystem {
public:
    struct LocationData {
        qint64 id = -1;
        QString path;
        bool isDefault = false;
    };

    struct FileData {
        qint64 id = -1;
        QString name;
        QString path;
        qint64 locationId = -1;
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

    // Default directories
    QString defaultRoot() const;
    QString defaultProjectsPath() const;
    QString databasePath() const;

    // Registered locations
    QVector<LocationData> locations() const;

    FsResult registerLocation(const QString& path, qint64& outId) const;
    FsResult unregisterLocation(qint64 locationId) const;

    // Registered documents
    QVector<FileData> files() const;

    // Registers an existing .ourp document. Its parent directory is
    // registered automatically when it has not been registered yet.
    FsResult registerFile(const QString& fullPath, qint64& outId) const;

    // Creates an empty .ourp document in a registered location.
    // The ".ourp" extension is added when it is omitted.
    FsResult createFile(qint64 locationId,
                        const QString& fileName,
                        qint64& outId) const;

    FsResult openFile(qint64 fileId, FileData& outData) const;
    FsResult getFileIdByPath(const QString& fullPath, qint64& outId) const;
    FsResult getFileData(qint64 fileId, UI::FileData& outData) const;

    // Removes the physical document and its registry entry.
    FsResult removeFile(qint64 fileId) const;

    // Rename/move preserve the document ID.
    FsResult renameFile(qint64 fileId, const QString& newName) const;
    FsResult moveFile(qint64 fileId,
                      qint64 newLocationId,
                      const QString& newName = QString()) const;

private:
    QString rootPath_;
    QString projectsDir_;
    QString settingsDir_;
    QString databasePath_;
    QString connectionName_;

    mutable QSqlDatabase database_;

    bool initialize();
    FsResult registerLocationInternal(const QString& path,
                                      qint64& outId,
                                      bool isDefault) const;

    static QString normalizePath(const QString& path);
    static QString pathKey(const QString& path);
    static bool isValidFileName(const QString& fileName);
    static QString normalizedDocumentName(const QString& fileName);
};

} // namespace UI

#endif // OURPAINT_DESKTOP_UI_FILESYSTEM_H