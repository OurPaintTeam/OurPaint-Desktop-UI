#include "FileSystem.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariant>
#include <QXmlStreamWriter>

#include <atomic>

namespace {

bool isOurPaintFile(const QFileInfo& info) {
    return info.isFile() &&
           info.suffix().compare(QStringLiteral("ourp"), Qt::CaseInsensitive) == 0;
}

} // namespace

UI::FileSystem::FileSystem() {
    const QString documents =
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    rootPath_ = normalizePath(documents + QStringLiteral("/OurPaint"));
    projectsDir_ = normalizePath(rootPath_ + QStringLiteral("/Projects"));
    settingsDir_ = normalizePath(rootPath_ + QStringLiteral("/Settings"));
    databasePath_ = normalizePath(settingsDir_ + QStringLiteral("/filesystem.db"));

    static std::atomic<quint64> connectionCounter{0};
    connectionName_ = QStringLiteral("OurPaintFileSystem_%1")
                          .arg(++connectionCounter);

    database_ = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName_);
    database_.setDatabaseName(databasePath_);

    initialize();
}

UI::FileSystem::~FileSystem() {
    if (database_.isValid()) {
        database_.close();
    }

    database_ = QSqlDatabase();
    if (!connectionName_.isEmpty()) {
        QSqlDatabase::removeDatabase(connectionName_);
    }
}

QString UI::FileSystem::defaultRoot() const {
    return QDir::toNativeSeparators(rootPath_);
}

QString UI::FileSystem::defaultProjectsPath() const {
    return QDir::toNativeSeparators(projectsDir_);
}

QString UI::FileSystem::databasePath() const {
    return QDir::toNativeSeparators(databasePath_);
}

QString UI::FileSystem::normalizePath(const QString& path) {
    if (path.trimmed().isEmpty()) {
        return {};
    }

    return QDir::cleanPath(QFileInfo(path).absoluteFilePath());
}

QString UI::FileSystem::pathKey(const QString& path) {
    const QString normalized = QDir::fromNativeSeparators(normalizePath(path));
#ifdef Q_OS_WIN
    return normalized.toCaseFolded();
#else
    return normalized;
#endif
}

bool UI::FileSystem::isValidFileName(const QString& fileName) {
    if (fileName.trimmed().isEmpty() ||
        fileName == QStringLiteral(".") ||
        fileName == QStringLiteral("..")) {
        return false;
    }

    if (fileName.contains(QLatin1Char('/')) ||
        fileName.contains(QLatin1Char('\\')) ||
        fileName.contains(QChar::Null)) {
        return false;
    }

    return true;
}

QString UI::FileSystem::normalizedDocumentName(const QString& fileName) {
    QString result = fileName.trimmed();

    if (QFileInfo(result).suffix().isEmpty()) {
        result += QStringLiteral(".ourp");
    }

    if (QFileInfo(result).suffix().compare(QStringLiteral("ourp"),
                                           Qt::CaseInsensitive) != 0) {
        return {};
    }

    return result;
}

bool UI::FileSystem::initialize() {
    if (!QDir().mkpath(projectsDir_) || !QDir().mkpath(settingsDir_)) {
        return false;
    }

    if (!database_.open()) {
        return false;
    }

    QSqlQuery pragma(database_);
    if (!pragma.exec(QStringLiteral("PRAGMA foreign_keys = ON"))) {
        return false;
    }

    QSqlQuery createLocations(database_);
    if (!createLocations.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS RegisteredLocations ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "path TEXT NOT NULL,"
            "path_key TEXT NOT NULL UNIQUE,"
            "is_default INTEGER NOT NULL DEFAULT 0"
            ")"))) {
        return false;
    }

    QSqlQuery createFiles(database_);
    if (!createFiles.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS Files ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL,"
            "path TEXT NOT NULL,"
            "path_key TEXT NOT NULL UNIQUE,"
            "location_id INTEGER NULL,"
            "FOREIGN KEY(location_id) REFERENCES RegisteredLocations(id) "
            "ON DELETE SET NULL"
            ")"))) {
        return false;
    }

    // Keep exactly one default location: OurPaint/Projects.
    QSqlQuery findDefault(database_);
    findDefault.prepare(QStringLiteral(
        "SELECT id FROM RegisteredLocations WHERE is_default = 1 LIMIT 1"));

    if (!findDefault.exec()) {
        return false;
    }

    if (!findDefault.next()) {
        qint64 defaultId = -1;
        if (registerLocationInternal(projectsDir_, defaultId, true) != FsResult::Ok) {
            return false;
        }
    }

    return true;
}

UI::FileSystem::FsResult UI::FileSystem::registerLocationInternal(
    const QString& path,
    qint64& outId,
    bool isDefault) const {

    const QString normalized = normalizePath(path);
    if (normalized.isEmpty()) {
        return FsResult::InvalidArgument;
    }

    const QFileInfo info(normalized);
    if (!info.exists() || !info.isDir()) {
        return FsResult::NotFound;
    }

    const QString key = pathKey(normalized);

    QSqlQuery find(database_);
    find.prepare(QStringLiteral(
        "SELECT id FROM RegisteredLocations WHERE path_key = :path_key"));
    find.bindValue(QStringLiteral(":path_key"), key);

    if (!find.exec()) {
        return FsResult::IoError;
    }

    if (find.next()) {
        outId = find.value(0).toLongLong();

        if (isDefault) {
            QSqlQuery markDefault(database_);
            markDefault.prepare(QStringLiteral(
                "UPDATE RegisteredLocations SET is_default = 0"));
            if (!markDefault.exec()) {
                return FsResult::IoError;
            }

            QSqlQuery update(database_);
            update.prepare(QStringLiteral(
                "UPDATE RegisteredLocations SET is_default = 1, path = :path "
                "WHERE id = :id"));
            update.bindValue(QStringLiteral(":path"), normalized);
            update.bindValue(QStringLiteral(":id"), outId);
            if (!update.exec()) {
                return FsResult::IoError;
            }
        }

        return FsResult::Ok;
    }

    if (isDefault) {
        QSqlQuery clearDefault(database_);
        if (!clearDefault.exec(QStringLiteral(
                "UPDATE RegisteredLocations SET is_default = 0"))) {
            return FsResult::IoError;
        }
    }

    QSqlQuery insert(database_);
    insert.prepare(QStringLiteral(
        "INSERT INTO RegisteredLocations(path, path_key, is_default) "
        "VALUES(:path, :path_key, :is_default)"));
    insert.bindValue(QStringLiteral(":path"), normalized);
    insert.bindValue(QStringLiteral(":path_key"), key);
    insert.bindValue(QStringLiteral(":is_default"), isDefault ? 1 : 0);

    if (!insert.exec()) {
        return FsResult::IoError;
    }

    outId = insert.lastInsertId().toLongLong();
    return FsResult::Ok;
}

QVector<UI::FileSystem::LocationData> UI::FileSystem::locations() const {
    QVector<LocationData> result;

    if (!database_.isOpen()) {
        return result;
    }

    QSqlQuery query(database_);
    if (!query.exec(QStringLiteral(
            "SELECT id, path, is_default FROM RegisteredLocations "
            "ORDER BY is_default DESC, path COLLATE NOCASE"))) {
        return result;
    }

    while (query.next()) {
        LocationData location;
        location.id = query.value(0).toLongLong();
        location.path = QDir::toNativeSeparators(query.value(1).toString());
        location.isDefault = query.value(2).toInt() != 0;
        result.append(location);
    }

    return result;
}

UI::FileSystem::FsResult UI::FileSystem::registerLocation(
    const QString& path,
    qint64& outId) const {

    if (!database_.isOpen()) {
        return FsResult::IoError;
    }

    return registerLocationInternal(path, outId, false);
}

UI::FileSystem::FsResult UI::FileSystem::unregisterLocation(qint64 locationId) const {
    if (!database_.isOpen()) {
        return FsResult::IoError;
    }

    QSqlQuery query(database_);
    query.prepare(QStringLiteral(
        "DELETE FROM RegisteredLocations WHERE id = :id AND is_default = 0"));
    query.bindValue(QStringLiteral(":id"), locationId);

    if (!query.exec()) {
        return FsResult::IoError;
    }

    return query.numRowsAffected() > 0 ? FsResult::Ok : FsResult::NotFound;
}

QVector<UI::FileSystem::FileData> UI::FileSystem::files() const {
    QVector<FileData> result;

    if (!database_.isOpen()) {
        return result;
    }

    QSqlQuery query(database_);
    if (!query.exec(QStringLiteral(
            "SELECT id, name, path, location_id FROM Files "
            "ORDER BY name COLLATE NOCASE"))) {
        return result;
    }

    while (query.next()) {
        FileData file;
        file.id = query.value(0).toLongLong();
        file.name = query.value(1).toString();
        file.path = QDir::toNativeSeparators(query.value(2).toString());
        file.locationId = query.value(3).isNull()
                              ? -1
                              : query.value(3).toLongLong();
        result.append(file);
    }

    return result;
}

UI::FileSystem::FsResult UI::FileSystem::registerFile(
    const QString& fullPath,
    qint64& outId) const {

    if (!database_.isOpen()) {
        return FsResult::IoError;
    }

    const QString normalized = normalizePath(fullPath);
    const QFileInfo info(normalized);

    if (!info.exists()) {
        return FsResult::NotFound;
    }

    if (!isOurPaintFile(info)) {
        return FsResult::InvalidArgument;
    }

    const QString parentPath = normalizePath(info.absolutePath());
    qint64 locationId = -1;

    const FsResult locationResult =
        registerLocationInternal(parentPath, locationId, false);

    if (locationResult != FsResult::Ok) {
        return locationResult;
    }

    QSqlQuery insert(database_);
    insert.prepare(QStringLiteral(
        "INSERT INTO Files(name, path, path_key, location_id) "
        "VALUES(:name, :path, :path_key, :location_id)"));
    insert.bindValue(QStringLiteral(":name"), info.fileName());
    insert.bindValue(QStringLiteral(":path"), normalized);
    insert.bindValue(QStringLiteral(":path_key"), pathKey(normalized));
    insert.bindValue(QStringLiteral(":location_id"), locationId);

    if (!insert.exec()) {
        if (insert.lastError().text().contains(QStringLiteral("UNIQUE"),
                                               Qt::CaseInsensitive)) {
            QSqlQuery find(database_);
            find.prepare(QStringLiteral(
                "SELECT id FROM Files WHERE path_key = :path_key"));
            find.bindValue(QStringLiteral(":path_key"), pathKey(normalized));
            if (find.exec() && find.next()) {
                outId = find.value(0).toLongLong();
                return FsResult::AlreadyExists;
            }
        }
        return FsResult::IoError;
    }

    outId = insert.lastInsertId().toLongLong();
    return FsResult::Ok;
}

UI::FileSystem::FsResult UI::FileSystem::createFile(
    qint64 locationId,
    const QString& fileName,
    qint64& outId) const {

    if (!database_.isOpen()) {
        return FsResult::IoError;
    }

    if (!isValidFileName(fileName)) {
        return FsResult::InvalidArgument;
    }

    const QString name = normalizedDocumentName(fileName);
    if (name.isEmpty()) {
        return FsResult::InvalidArgument;
    }

    QSqlQuery locationQuery(database_);
    locationQuery.prepare(QStringLiteral(
        "SELECT path FROM RegisteredLocations WHERE id = :id"));
    locationQuery.bindValue(QStringLiteral(":id"), locationId);

    if (!locationQuery.exec()) {
        return FsResult::IoError;
    }

    if (!locationQuery.next()) {
        return FsResult::NotFound;
    }

    const QString directory = locationQuery.value(0).toString();
    const QString fullPath = normalizePath(directory + QLatin1Char('/') + name);

    if (QFileInfo::exists(fullPath)) {
        return FsResult::AlreadyExists;
    }

    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly)) {
        return FsResult::IoError;
    }

    // Keep the same basic document envelope used by the previous
    // createTab() implementation. The editor can populate it afterwards.
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement(QStringLiteral("tab"));
    xml.writeAttribute(QStringLiteral("name"), QFileInfo(name).completeBaseName());
    xml.writeEndElement();
    xml.writeEndDocument();
    file.close();

    if (file.error() != QFileDevice::NoError) {
        QFile::remove(fullPath);
        return FsResult::IoError;
    }

    QSqlQuery insert(database_);
    insert.prepare(QStringLiteral(
        "INSERT INTO Files(name, path, path_key, location_id) "
        "VALUES(:name, :path, :path_key, :location_id)"));
    insert.bindValue(QStringLiteral(":name"), name);
    insert.bindValue(QStringLiteral(":path"), fullPath);
    insert.bindValue(QStringLiteral(":path_key"), pathKey(fullPath));
    insert.bindValue(QStringLiteral(":location_id"), locationId);

    if (!insert.exec()) {
        QFile::remove(fullPath);

        if (insert.lastError().text().contains(QStringLiteral("UNIQUE"),
                                               Qt::CaseInsensitive)) {
            return FsResult::AlreadyExists;
        }
        return FsResult::IoError;
    }

    outId = insert.lastInsertId().toLongLong();
    return FsResult::Ok;
}

UI::FileSystem::FsResult UI::FileSystem::openFile(
    qint64 fileId,
    FileData& outData) const {

    if (!database_.isOpen()) {
        return FsResult::IoError;
    }

    QSqlQuery query(database_);
    query.prepare(QStringLiteral(
        "SELECT id, name, path, location_id FROM Files WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), fileId);

    if (!query.exec()) {
        return FsResult::IoError;
    }

    if (!query.next()) {
        return FsResult::NotFound;
    }

    const QString path = query.value(2).toString();
    if (!QFileInfo::exists(path)) {
        return FsResult::NotFound;
    }

    outData.id = query.value(0).toLongLong();
    outData.name = query.value(1).toString();
    outData.path = QDir::toNativeSeparators(path);
    outData.locationId = query.value(3).isNull()
                             ? -1
                             : query.value(3).toLongLong();

    return FsResult::Ok;
}

UI::FileSystem::FsResult UI::FileSystem::getFileData(
    qint64 fileId,
    UI::FileData& outData) const {

    if (!database_.isOpen()) {
        return FsResult::IoError;
    }

    QSqlQuery query(database_);
    query.prepare(QStringLiteral(
        "SELECT id, name, path FROM Files WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), fileId);

    if (!query.exec()) {
        return FsResult::IoError;
    }

    if (!query.next()) {
        return FsResult::NotFound;
    }

    outData.id   = query.value(0).toLongLong();
    outData.name = query.value(1).toString();
    outData.path = QDir::toNativeSeparators(query.value(2).toString());

    return FsResult::Ok;
}

UI::FileSystem::FsResult UI::FileSystem::getFileIdByPath(
    const QString& fullPath,
    qint64& outId) const {

    if (!database_.isOpen()) {
        return FsResult::IoError;
    }

    const QString normalized = normalizePath(fullPath);

    QSqlQuery query(database_);
    query.prepare(QStringLiteral(
        "SELECT id FROM Files WHERE path_key = :path_key"));
    query.bindValue(QStringLiteral(":path_key"), pathKey(normalized));

    if (!query.exec()) {
        return FsResult::IoError;
    }

    if (!query.next()) {
        return FsResult::NotFound;
    }

    outId = query.value(0).toLongLong();
    return FsResult::Ok;
}

UI::FileSystem::FsResult UI::FileSystem::removeFile(qint64 fileId) const {
    if (!database_.isOpen()) {
        return FsResult::IoError;
    }

    QSqlQuery find(database_);
    find.prepare(QStringLiteral("SELECT path FROM Files WHERE id = :id"));
    find.bindValue(QStringLiteral(":id"), fileId);

    if (!find.exec()) {
        return FsResult::IoError;
    }

    if (!find.next()) {
        return FsResult::NotFound;
    }

    const QString path = find.value(0).toString();

    if (QFileInfo::exists(path) && !QFile::remove(path)) {
        return FsResult::IoError;
    }

    QSqlQuery remove(database_);
    remove.prepare(QStringLiteral("DELETE FROM Files WHERE id = :id"));
    remove.bindValue(QStringLiteral(":id"), fileId);

    if (!remove.exec()) {
        return FsResult::IoError;
    }

    return remove.numRowsAffected() > 0 ? FsResult::Ok : FsResult::NotFound;
}

UI::FileSystem::FsResult UI::FileSystem::renameFile(
    qint64 fileId,
    const QString& newName) const {

    if (!database_.isOpen()) {
        return FsResult::IoError;
    }

    if (!isValidFileName(newName)) {
        return FsResult::InvalidArgument;
    }

    const QString name = normalizedDocumentName(newName);
    if (name.isEmpty()) {
        return FsResult::InvalidArgument;
    }

    FileData current;
    const FsResult openResult = openFile(fileId, current);
    if (openResult != FsResult::Ok) {
        return openResult;
    }

    const QString oldPath = normalizePath(current.path);
    const QString newPath =
        normalizePath(QFileInfo(oldPath).absolutePath() + QLatin1Char('/') + name);

    if (pathKey(oldPath) == pathKey(newPath)) {
        return FsResult::Ok;
    }

    if (QFileInfo::exists(newPath)) {
        return FsResult::AlreadyExists;
    }

    if (!QFile::rename(oldPath, newPath)) {
        return FsResult::IoError;
    }

    QSqlQuery update(database_);
    update.prepare(QStringLiteral(
        "UPDATE Files SET name = :name, path = :path, path_key = :path_key "
        "WHERE id = :id"));
    update.bindValue(QStringLiteral(":name"), name);
    update.bindValue(QStringLiteral(":path"), newPath);
    update.bindValue(QStringLiteral(":path_key"), pathKey(newPath));
    update.bindValue(QStringLiteral(":id"), fileId);

    if (!update.exec()) {
        QFile::rename(newPath, oldPath);
        return FsResult::IoError;
    }

    return FsResult::Ok;
}

UI::FileSystem::FsResult UI::FileSystem::moveFile(
    qint64 fileId,
    qint64 newLocationId,
    const QString& newName) const {

    if (!database_.isOpen()) {
        return FsResult::IoError;
    }

    FileData current;
    const FsResult openResult = openFile(fileId, current);
    if (openResult != FsResult::Ok) {
        return openResult;
    }

    QSqlQuery locationQuery(database_);
    locationQuery.prepare(QStringLiteral(
        "SELECT path FROM RegisteredLocations WHERE id = :id"));
    locationQuery.bindValue(QStringLiteral(":id"), newLocationId);

    if (!locationQuery.exec()) {
        return FsResult::IoError;
    }

    if (!locationQuery.next()) {
        return FsResult::NotFound;
    }

    QString targetName = newName.trimmed().isEmpty() ? current.name : newName;
    if (!isValidFileName(targetName)) {
        return FsResult::InvalidArgument;
    }

    targetName = normalizedDocumentName(targetName);
    if (targetName.isEmpty()) {
        return FsResult::InvalidArgument;
    }

    const QString oldPath = normalizePath(current.path);
    const QString newPath = normalizePath(
        locationQuery.value(0).toString() + QLatin1Char('/') + targetName);

    if (pathKey(oldPath) == pathKey(newPath)) {
        return FsResult::Ok;
    }

    if (QFileInfo::exists(newPath)) {
        return FsResult::AlreadyExists;
    }

    // QFile::rename works on the same volume. For cross-volume moves,
    // copy first and remove the source only after the copy succeeds.
    if (!QFile::rename(oldPath, newPath)) {
        if (!QFile::copy(oldPath, newPath)) {
            return FsResult::IoError;
        }

        if (!QFile::remove(oldPath)) {
            QFile::remove(newPath);
            return FsResult::IoError;
        }

    }

    QSqlQuery update(database_);
    update.prepare(QStringLiteral(
        "UPDATE Files SET name = :name, path = :path, path_key = :path_key, "
        "location_id = :location_id WHERE id = :id"));
    update.bindValue(QStringLiteral(":name"), targetName);
    update.bindValue(QStringLiteral(":path"), newPath);
    update.bindValue(QStringLiteral(":path_key"), pathKey(newPath));
    update.bindValue(QStringLiteral(":location_id"), newLocationId);
    update.bindValue(QStringLiteral(":id"), fileId);

    if (!update.exec()) {
        // Best-effort rollback of the physical move.
        if (!QFile::rename(newPath, oldPath)) {
            if (QFile::copy(newPath, oldPath)) {
                QFile::remove(newPath);
            }
        }
        return FsResult::IoError;
    }

    return FsResult::Ok;
}
