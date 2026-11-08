#include "FileSystem.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSqlError>
#include <QSaveFile>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariant>

#include <atomic>

namespace {

bool execSql(QSqlDatabase& db, const QString& sql) {
    QSqlQuery query(db);
    return query.exec(sql);
}

} // namespace

UI::FileSystem::FileSystem() {
    const QString documents =
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

    rootPath_ = normalizePath(QDir(documents).filePath(QStringLiteral("OurPaint")));
    projectsDir_ = normalizePath(QDir(rootPath_).filePath(QStringLiteral("Projects")));
    settingsDir_ = normalizePath(QDir(rootPath_).filePath(QStringLiteral("Settings")));
    scriptsDir_ = normalizePath(QDir(rootPath_).filePath(QStringLiteral("Scripts")));
    databasePath_ =
        normalizePath(QDir(settingsDir_).filePath(QStringLiteral("filesystem.db")));

    static std::atomic<quint64> connectionCounter{0};
    connectionName_ =
        QStringLiteral("OurPaintFileSystem_%1").arg(++connectionCounter);

    database_ = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), connectionName_);
    database_.setDatabaseName(databasePath_);

    initialized_ = initialize();
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

QString UI::FileSystem::rootPath() const {
    return QDir::toNativeSeparators(rootPath_);
}

QString UI::FileSystem::projectsPath() const {
    return QDir::toNativeSeparators(projectsDir_);
}

QString UI::FileSystem::settingsPath() const {
    return QDir::toNativeSeparators(settingsDir_);
}

QString UI::FileSystem::scriptsPath() const {
    return QDir::toNativeSeparators(scriptsDir_);
}

QString UI::FileSystem::databasePath() const {
    return QDir::toNativeSeparators(databasePath_);
}

bool UI::FileSystem::isInitialized() const {
    return initialized_ && database_.isOpen();
}

QString UI::FileSystem::normalizePath(const QString& path) {
    if (path.trimmed().isEmpty()) {
        return {};
    }
    return QDir::cleanPath(QFileInfo(path).absoluteFilePath());
}

QString UI::FileSystem::pathKey(const QString& path) {
    const QString normalized =
        QDir::fromNativeSeparators(normalizePath(path));
#ifdef Q_OS_WIN
    return normalized.toCaseFolded();
#else
    return normalized;
#endif
}

bool UI::FileSystem::isOurPaintFile(const QString& path) {
    const QFileInfo info(path);
    return info.exists() && info.isFile() &&
           info.suffix().compare(QStringLiteral("ourp"), Qt::CaseInsensitive) == 0;
}

QString UI::FileSystem::fullPathFor(const QString& directory, const QString& baseName) {
    return QDir(directory).filePath(baseName + QStringLiteral(".ourp"));
}

UI::FileSystem::FsResult UI::FileSystem::validateProjectName(
    const QString& name, QString& outBaseName) const {
    outBaseName = name.trimmed();

    if (outBaseName.isEmpty() ||
        outBaseName == QStringLiteral(".") ||
        outBaseName == QStringLiteral("..") ||
        outBaseName.contains(QLatin1Char('/')) ||
        outBaseName.contains(QLatin1Char('\\')) ||
        outBaseName.contains(QChar::Null)) {
        return FsResult::InvalidArgument;
    }

    if (outBaseName.endsWith(QStringLiteral(".ourp"), Qt::CaseInsensitive)) {
        outBaseName.chop(5);
    }

    if (outBaseName.isEmpty()) {
        return FsResult::InvalidArgument;
    }

    return FsResult::Ok;
}

bool UI::FileSystem::initialize() {
    initialized_ = false;

    if (!QDir().mkpath(projectsDir_) ||
        !QDir().mkpath(settingsDir_) ||
        !QDir().mkpath(scriptsDir_)) {
        return false;
    }

    if (!database_.open()) {
        return false;
    }

    if (!execSql(database_, QStringLiteral("PRAGMA foreign_keys = ON"))) {
        return false;
    }

    if (!execSql(database_, QStringLiteral(
            "CREATE TABLE IF NOT EXISTS Paths ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "path TEXT NOT NULL UNIQUE"
            ")"))) {
        return false;
    }

    if (!execSql(database_, QStringLiteral(
            "CREATE TABLE IF NOT EXISTS Projects ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL COLLATE NOCASE,"
            "path_id INTEGER NOT NULL,"
            "UNIQUE(path_id, name),"
            "FOREIGN KEY(path_id) REFERENCES Paths(id) ON DELETE CASCADE"
            ")"))) {
        return false;
    }

    QSqlQuery ensurePath(database_);
    ensurePath.prepare(QStringLiteral(
        "INSERT OR IGNORE INTO Paths(path) VALUES(:path)"));
    ensurePath.bindValue(QStringLiteral(":path"), projectsDir_);
    if (!ensurePath.exec()) {
        return false;
    }

    QSqlQuery legacyTable(database_);
    if (!legacyTable.exec(QStringLiteral(
            "SELECT name FROM sqlite_master WHERE type = 'table' "
            "AND name = 'Files'"))) {
        return false;
    }
    const bool hasLegacyFiles = legacyTable.next();

    if (hasLegacyFiles) {
        if (!database_.transaction()) {
            return false;
        }

        QSqlQuery legacyFiles(database_);
        if (!legacyFiles.exec(QStringLiteral(
                "SELECT id, name, path FROM Files ORDER BY id"))) {
            database_.rollback();
            return false;
        }

        while (legacyFiles.next()) {
            const qint64 oldId = legacyFiles.value(0).toLongLong();
            const QString oldName = legacyFiles.value(1).toString();
            const QString oldFullPath = normalizePath(legacyFiles.value(2).toString());
            const QFileInfo oldInfo(oldFullPath);

            if (!isOurPaintFile(oldFullPath) ||
                pathKey(oldInfo.absolutePath()) != pathKey(projectsDir_)) {
                continue;
            }

            QSqlQuery migrated(database_);
            migrated.prepare(QStringLiteral(
                "INSERT OR IGNORE INTO Projects(id, name, path_id) "
                "SELECT :id, :name, id FROM Paths WHERE path = :path"));
            migrated.bindValue(QStringLiteral(":id"), oldId);
            migrated.bindValue(QStringLiteral(":name"),
                               oldName.isEmpty() ? oldInfo.completeBaseName()
                                                 : oldName);
            migrated.bindValue(QStringLiteral(":path"), projectsDir_);
            if (!migrated.exec()) {
                database_.rollback();
                return false;
            }
        }

        if (!execSql(database_, QStringLiteral("DROP TABLE Files")) ||
            !execSql(database_, QStringLiteral("DROP TABLE RegisteredLocations"))) {
            database_.rollback();
            return false;
        }
        if (!database_.commit()) {
            database_.rollback();
            return false;
        }
    }

    QSqlQuery findPath(database_);
    findPath.prepare(QStringLiteral(
        "SELECT id FROM Paths WHERE path = :path LIMIT 1"));
    findPath.bindValue(QStringLiteral(":path"), projectsDir_);

    if (!findPath.exec()) {
        return false;
    }

    if (!findPath.next()) {
        QSqlQuery insertPath(database_);
        insertPath.prepare(QStringLiteral(
            "INSERT INTO Paths(path) VALUES(:path)"));
        insertPath.bindValue(QStringLiteral(":path"), projectsDir_);
        if (!insertPath.exec()) {
            return false;
        }
    }

    if (migrateLegacyNames() != FsResult::Ok) {
        return false;
    }

    initialized_ = true;
    if (synchronizeInternal() != FsResult::Ok) {
        initialized_ = false;
        return false;
    }
    return true;
}

UI::FileSystem::FsResult UI::FileSystem::migrateLegacyNames() {
    QSqlQuery selectOld(database_);
    if (!selectOld.exec(QStringLiteral(
            "SELECT id, path_id, name FROM Projects "
            "WHERE name LIKE '%.ourp'"))) {
        return FsResult::IoError;
    }

    struct OldRow { qint64 id; qint64 pathId; QString baseName; };
    QVector<OldRow> toMigrate;
    while (selectOld.next()) {
        OldRow r;
        r.id       = selectOld.value(0).toLongLong();
        r.pathId   = selectOld.value(1).toLongLong();
        r.baseName = selectOld.value(2).toString();
        r.baseName.chop(5);
        if (!r.baseName.isEmpty()) {
            toMigrate.append(r);
        }
    }

    if (toMigrate.isEmpty()) {
        return FsResult::Ok;
    }

    if (!database_.transaction()) {
        return FsResult::IoError;
    }

    for (const auto& r : toMigrate) {
        QSqlQuery check(database_);
        check.prepare(QStringLiteral(
            "SELECT 1 FROM Projects "
            "WHERE path_id = :pid AND name = :name AND id <> :id LIMIT 1"));
        check.bindValue(QStringLiteral(":pid"), r.pathId);
        check.bindValue(QStringLiteral(":name"), r.baseName);
        check.bindValue(QStringLiteral(":id"), r.id);
        if (!check.exec()) {
            database_.rollback();
            return FsResult::IoError;
        }

        if (check.next()) {
            QSqlQuery del(database_);
            del.prepare(QStringLiteral("DELETE FROM Projects WHERE id = :id"));
            del.bindValue(QStringLiteral(":id"), r.id);
            if (!del.exec()) {
                database_.rollback();
                return FsResult::IoError;
            }
        } else {
            QSqlQuery upd(database_);
            upd.prepare(QStringLiteral(
                "UPDATE Projects SET name = :name WHERE id = :id"));
            upd.bindValue(QStringLiteral(":name"), r.baseName);
            upd.bindValue(QStringLiteral(":id"), r.id);
            if (!upd.exec()) {
                database_.rollback();
                return FsResult::IoError;
            }
        }
    }

    if (!database_.commit()) {
        database_.rollback();
        return FsResult::IoError;
    }
    return FsResult::Ok;
}

qint64 UI::FileSystem::defaultPathId() const {
    if (!isInitialized()) {
        return -1;
    }

    QSqlQuery query(database_);
    query.prepare(QStringLiteral("SELECT id FROM Paths WHERE path = :path"));
    query.bindValue(QStringLiteral(":path"), projectsDir_);

    if (!query.exec() || !query.next()) {
        return -1;
    }
    return query.value(0).toLongLong();
}

UI::FileSystem::FsResult UI::FileSystem::getPath(
    qint64 pathId, PathData& outPath) const {
    if (!isInitialized()) {
        return FsResult::IoError;
    }

    QSqlQuery query(database_);
    query.prepare(QStringLiteral("SELECT id, path FROM Paths WHERE id = :id"));
    query.bindValue(QStringLiteral(":id"), pathId);

    if (!query.exec()) {
        return FsResult::IoError;
    }
    if (!query.next()) {
        return FsResult::NotFound;
    }

    outPath.id = query.value(0).toLongLong();
    outPath.path = QDir::toNativeSeparators(query.value(1).toString());
    return FsResult::Ok;
}

UI::FileSystem::FsResult UI::FileSystem::synchronize() {
    if (!isInitialized()) {
        return FsResult::IoError;
    }
    return synchronizeInternal();
}

UI::FileSystem::FsResult UI::FileSystem::synchronizeInternal() {
    if (!QDir().mkpath(projectsDir_) ||
        !QDir().mkpath(settingsDir_) ||
        !QDir().mkpath(scriptsDir_)) {
        return FsResult::IoError;
    }

    QSqlQuery pathsQuery(database_);
    if (!pathsQuery.exec(QStringLiteral("SELECT id, path FROM Paths"))) {
        return FsResult::IoError;
    }

    QVector<qint64> stalePathIds;
    while (pathsQuery.next()) {
        const qint64 id = pathsQuery.value(0).toLongLong();
        const QString path = pathsQuery.value(1).toString();
        const QFileInfo info(path);
        if (!info.exists() || !info.isDir()) {
            stalePathIds.append(id);
        }
    }

    for (qint64 id : stalePathIds) {
        QSqlQuery removePath(database_);
        removePath.prepare(QStringLiteral("DELETE FROM Paths WHERE id = :id"));
        removePath.bindValue(QStringLiteral(":id"), id);
        if (!removePath.exec()) {
            return FsResult::IoError;
        }
    }

    qint64 defaultId = defaultPathId();
    if (defaultId < 0) {
        QSqlQuery insertPath(database_);
        insertPath.prepare(QStringLiteral("INSERT INTO Paths(path) VALUES(:path)"));
        insertPath.bindValue(QStringLiteral(":path"), projectsDir_);
        if (!insertPath.exec()) {
            return FsResult::IoError;
        }
        defaultId = insertPath.lastInsertId().toLongLong();
    }

    QSqlQuery projectQuery(database_);
    if (!projectQuery.exec(QStringLiteral(
            "SELECT Projects.id, Projects.name, Paths.path "
            "FROM Projects INNER JOIN Paths ON Paths.id = Projects.path_id"))) {
        return FsResult::IoError;
    }

    QVector<qint64> staleProjectIds;
    while (projectQuery.next()) {
        const qint64 id = projectQuery.value(0).toLongLong();
        const QString name = projectQuery.value(1).toString();
        const QString directory = projectQuery.value(2).toString();
        const QString fullPath = fullPathFor(directory, name);

        if (!isOurPaintFile(fullPath)) {
            staleProjectIds.append(id);
        }
    }

    for (qint64 id : staleProjectIds) {
        QSqlQuery removeProject(database_);
        removeProject.prepare(QStringLiteral("DELETE FROM Projects WHERE id = :id"));
        removeProject.bindValue(QStringLiteral(":id"), id);
        if (!removeProject.exec()) {
            return FsResult::IoError;
        }
    }

    const QDir directory(projectsDir_);
    const QFileInfoList entries = directory.entryInfoList(
        QDir::Files | QDir::NoDotAndDotDot, QDir::Name | QDir::IgnoreCase);

    for (const QFileInfo& info : entries) {
        if (!isOurPaintFile(info.absoluteFilePath())) {
            continue;
        }

        const QString baseName = info.completeBaseName();

        QSqlQuery findProject(database_);
        findProject.prepare(QStringLiteral(
            "SELECT id FROM Projects WHERE path_id = :path_id AND name = :name"));
        findProject.bindValue(QStringLiteral(":path_id"), defaultId);
        findProject.bindValue(QStringLiteral(":name"), baseName);

        if (!findProject.exec()) {
            return FsResult::IoError;
        }
        if (findProject.next()) {
            continue;
        }

        QSqlQuery insertProjectQuery(database_);
        insertProjectQuery.prepare(QStringLiteral(
            "INSERT INTO Projects(name, path_id) VALUES(:name, :path_id)"));
        insertProjectQuery.bindValue(QStringLiteral(":name"), baseName);
        insertProjectQuery.bindValue(QStringLiteral(":path_id"), defaultId);
        if (!insertProjectQuery.exec()) {
            return FsResult::IoError;
        }
    }

    return FsResult::Ok;
}

QVector<UI::FileSystem::ProjectData> UI::FileSystem::projects()  {
    QVector<ProjectData> result;
    if (!isInitialized()) {
        return result;
    }

    auto* self = const_cast<FileSystem*>(this);
    if (self->synchronizeInternal() != FsResult::Ok) {
        return result;
    }

    QSqlQuery query(database_);
    if (!query.exec(QStringLiteral(
            "SELECT Projects.id, Projects.name, Paths.path, Projects.path_id "
            "FROM Projects INNER JOIN Paths ON Paths.id = Projects.path_id "
            "ORDER BY Projects.name COLLATE NOCASE"))) {
        return result;
    }

    while (query.next()) {
        ProjectData project;
        project.id = query.value(0).toLongLong();
        project.name = query.value(1).toString();
        project.path = QDir::toNativeSeparators(
            fullPathFor(query.value(2).toString(), project.name));
        project.pathId = query.value(3).toLongLong();
        result.append(project);
    }
    return result;
}

UI::FileSystem::FsResult UI::FileSystem::getProjectInternal(
    qint64 projectId, ProjectData& outProject)  {
    QSqlQuery query(database_);
    query.prepare(QStringLiteral(
        "SELECT Projects.id, Projects.name, Paths.path, Projects.path_id "
        "FROM Projects INNER JOIN Paths ON Paths.id = Projects.path_id "
        "WHERE Projects.id = :id"));
    query.bindValue(QStringLiteral(":id"), projectId);

    if (!query.exec()) {
        return FsResult::IoError;
    }
    if (!query.next()) {
        return FsResult::NotFound;
    }

    outProject.id = query.value(0).toLongLong();
    outProject.name = query.value(1).toString();
    outProject.path = QDir::toNativeSeparators(
        fullPathFor(query.value(2).toString(), outProject.name));
    outProject.pathId = query.value(3).toLongLong();
    return FsResult::Ok;
}

UI::FileSystem::FsResult UI::FileSystem::getProject(
    qint64 projectId, ProjectData& outProject)  {
    if (!isInitialized()) {
        return FsResult::IoError;
    }
    auto* self = const_cast<FileSystem*>(this);
    const FsResult syncResult = self->synchronizeInternal();
    if (syncResult != FsResult::Ok) {
        return syncResult;
    }
    return getProjectInternal(projectId, outProject);
}

UI::FileSystem::FsResult UI::FileSystem::getProjectPath(
    qint64 projectId, QString& outPath)  {
    ProjectData project;
    const FsResult result = getProject(projectId, project);
    if (result == FsResult::Ok) {
        outPath = project.path;
    }
    return result;
}

UI::FileSystem::FsResult UI::FileSystem::getProjectIdByPath(
    const QString& fullPath, qint64& outId) const {
    if (!isInitialized()) {
        return FsResult::IoError;
    }
    auto* self = const_cast<FileSystem*>(this);
    const FsResult syncResult = self->synchronizeInternal();
    if (syncResult != FsResult::Ok) {
        return syncResult;
    }

    const QString normalized = normalizePath(fullPath);
    if (pathKey(QFileInfo(normalized).absolutePath()) != pathKey(projectsDir_)) {
        return FsResult::NotFound;
    }

    QSqlQuery query(database_);
    query.prepare(QStringLiteral(
        "SELECT Projects.id FROM Projects "
        "INNER JOIN Paths ON Paths.id = Projects.path_id "
        "WHERE Paths.path = :path AND Projects.name = :name"));
    query.bindValue(QStringLiteral(":path"), projectsDir_);
    query.bindValue(QStringLiteral(":name"),
                    QFileInfo(normalized).completeBaseName());

    if (!query.exec()) {
        return FsResult::IoError;
    }
    if (!query.next()) {
        return FsResult::NotFound;
    }
    outId = query.value(0).toLongLong();
    return FsResult::Ok;
}

UI::FileSystem::FsResult UI::FileSystem::insertProject(
    const QString& baseName, qint64& outId) {
    const qint64 pathId = defaultPathId();
    if (pathId < 0) {
        return FsResult::IoError;
    }

    QSqlQuery insert(database_);
    insert.prepare(QStringLiteral(
        "INSERT INTO Projects(name, path_id) VALUES(:name, :path_id)"));
    insert.bindValue(QStringLiteral(":name"), baseName);
    insert.bindValue(QStringLiteral(":path_id"), pathId);

    if (!insert.exec()) {
        return insert.lastError().text().contains(
                   QStringLiteral("UNIQUE"), Qt::CaseInsensitive)
                   ? FsResult::AlreadyExists
                   : FsResult::IoError;
    }

    outId = insert.lastInsertId().toLongLong();
    return FsResult::Ok;
}

UI::FileSystem::FsResult UI::FileSystem::createProject(
    const QString& name, qint64& outId) {
    if (!isInitialized()) {
        return FsResult::IoError;
    }
    FsResult result = synchronizeInternal();
    if (result != FsResult::Ok) {
        return result;
    }

    QString baseName;
    result = validateProjectName(name, baseName);
    if (result != FsResult::Ok) {
        return result;
    }

    const qint64 pathId = defaultPathId();
    if (pathId < 0) {
        return FsResult::IoError;
    }

    {
        QSqlQuery check(database_);
        check.prepare(QStringLiteral(
            "SELECT 1 FROM Projects "
            "WHERE path_id = :path_id AND name = :name LIMIT 1"));
        check.bindValue(QStringLiteral(":path_id"), pathId);
        check.bindValue(QStringLiteral(":name"), baseName);
        if (!check.exec()) {
            return FsResult::IoError;
        }
        if (check.next()) {
            return FsResult::AlreadyExists;
        }
    }

    const QString fullPath = fullPathFor(projectsDir_, baseName);
    if (QFileInfo::exists(fullPath)) {
        return FsResult::AlreadyExists;
    }

    {
        QSaveFile file(fullPath);
        if (!file.open(QIODevice::WriteOnly)) {
            return FsResult::IoError;
        }
        if (!file.commit()) {
            return FsResult::IoError;
        }
    }

    result = insertProject(baseName, outId);
    if (result != FsResult::Ok) {
        QFile::remove(fullPath);
        return result;
    }
    return FsResult::Ok;
}

UI::FileSystem::FsResult UI::FileSystem::registerProject(
    const QString& fullPath, qint64& outId) {
    if (!isInitialized()) {
        return FsResult::IoError;
    }
    FsResult result = synchronizeInternal();
    if (result != FsResult::Ok) {
        return result;
    }

    const QString normalized = normalizePath(fullPath);
    const QFileInfo info(normalized);
    if (!info.exists() || !info.isFile()) {
        return FsResult::NotFound;
    }
    if (!isOurPaintFile(normalized) ||
        pathKey(info.absolutePath()) != pathKey(projectsDir_)) {
        return FsResult::InvalidArgument;
    }

    const QString baseName = info.completeBaseName();

    QSqlQuery find(database_);
    find.prepare(QStringLiteral(
        "SELECT id FROM Projects WHERE path_id = :path_id AND name = :name"));
    find.bindValue(QStringLiteral(":path_id"), defaultPathId());
    find.bindValue(QStringLiteral(":name"), baseName);
    if (!find.exec()) {
        return FsResult::IoError;
    }
    if (find.next()) {
        outId = find.value(0).toLongLong();
        return FsResult::AlreadyExists;
    }

    return insertProject(baseName, outId);
}

UI::FileSystem::FsResult UI::FileSystem::renameProject(
    qint64 projectId, const QString& newName) {
    if (!isInitialized()) {
        return FsResult::IoError;
    }
    FsResult result = synchronizeInternal();
    if (result != FsResult::Ok) {
        return result;
    }

    QString newBaseName;
    result = validateProjectName(newName, newBaseName);
    if (result != FsResult::Ok) {
        return result;
    }

    ProjectData project;
    result = getProjectInternal(projectId, project);
    if (result != FsResult::Ok) {
        return result;
    }
    if (project.name == newBaseName) {
        return FsResult::Ok;
    }

    {
        QSqlQuery check(database_);
        check.prepare(QStringLiteral(
            "SELECT 1 FROM Projects "
            "WHERE path_id = :path_id AND name = :name AND id <> :id LIMIT 1"));
        check.bindValue(QStringLiteral(":path_id"), project.pathId);
        check.bindValue(QStringLiteral(":name"), newBaseName);
        check.bindValue(QStringLiteral(":id"), projectId);
        if (!check.exec()) {
            return FsResult::IoError;
        }
        if (check.next()) {
            return FsResult::AlreadyExists;
        }
    }

    const QString newPath = fullPathFor(projectsDir_, newBaseName);
    if (QFileInfo::exists(newPath)) {
        return FsResult::AlreadyExists;
    }
    if (!QFile::rename(project.path, newPath)) {
        return FsResult::IoError;
    }

    if (!database_.transaction()) {
        QFile::rename(newPath, project.path);
        return FsResult::IoError;
    }

    QSqlQuery update(database_);
    update.prepare(QStringLiteral(
        "UPDATE Projects SET name = :name WHERE id = :id"));
    update.bindValue(QStringLiteral(":name"), newBaseName);
    update.bindValue(QStringLiteral(":id"), projectId);

    if (!update.exec() || !database_.commit()) {
        database_.rollback();
        QFile::rename(newPath, project.path);
        return FsResult::IoError;
    }
    return FsResult::Ok;
}

UI::FileSystem::FsResult UI::FileSystem::removeProject(qint64 projectId) {
    if (!isInitialized()) {
        return FsResult::IoError;
    }
    FsResult result = synchronizeInternal();
    if (result != FsResult::Ok) {
        return result;
    }

    ProjectData project;
    result = getProjectInternal(projectId, project);
    if (result != FsResult::Ok) {
        return result;
    }

    if (QFileInfo::exists(project.path) && !QFile::remove(project.path)) {
        return FsResult::IoError;
    }

    QSqlQuery remove(database_);
    remove.prepare(QStringLiteral("DELETE FROM Projects WHERE id = :id"));
    remove.bindValue(QStringLiteral(":id"), projectId);
    if (!remove.exec()) {
        return FsResult::IoError;
    }
    return remove.numRowsAffected() > 0 ? FsResult::Ok : FsResult::NotFound;
}