#include "FileSystem.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QUuid>
#include <QXmlStreamWriter>

namespace {
    QString toStoragePath(const QString& path) {
        return QDir::fromNativeSeparators(QDir::cleanPath(path));
    }


    QString toComparablePath(const QString& path) {
        return QDir::fromNativeSeparators(QDir::cleanPath(path));
    }


    QString toDisplayPath(const QString& path) {
        return QDir::toNativeSeparators(path);
    }


    QString toSystemPath(const QString& path) {
        return QDir::toNativeSeparators(path);
    }
} // namespace


UI::FileSystem::FileSystem() {
    const auto docs = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    rootPath_ = toSystemPath(docs + "/OurPaint/");
    projectsDir_ = toSystemPath(rootPath_ + "/Projects/");
    settingsDir_ = toSystemPath(rootPath_ + "/Settings/");
    projectsXml_ = toSystemPath(settingsDir_ + "/projects.xml");
    initialize();
}


QString UI::FileSystem::defaultRoot() const {
    return toDisplayPath(rootPath_);
}


QString UI::FileSystem::defaultProjectsPath() const {
    return toDisplayPath(projectsDir_);
}


QVector<QPair<QString, QString> > UI::FileSystem::projects() const {
    QVector<QPair<QString, QString> > result;

    for (const auto& p: loadProjects()) {
        if (auto systemPath = toSystemPath(p.path); QDir(systemPath).exists()) {
            result.append({p.name, toDisplayPath(p.path)});
        }
    }

    return result;
}


UI::FileSystem::FsResult UI::FileSystem::getProjectDataById(const QString& projectId, ProjectData& outData) const {
    return openProjectById(projectId, outData);
}


UI::FileSystem::FsResult UI::FileSystem::getProjectIdByPath(const QString& fullPath, QString& outProjectId) const {
    const auto comparablePath = toComparablePath(fullPath);

    auto projects = loadProjects();

    for (const auto& project: projects) {
        if (toComparablePath(project.path) == comparablePath) {
            outProjectId = project.id;
            return FsResult::Ok;
        }
    }

    return FsResult::NotFound;
}


UI::FileSystem::FsResult UI::FileSystem::createProject(const QString& fullPath, QString& outProjectId) const {
    const auto systemPath = toSystemPath(fullPath);

    if (const QDir dir(systemPath); !dir.exists()) {
        if (!dir.mkpath(".")) {
            return FsResult::IoError;
        }
    }

    const auto name = extractProjectNameFromPath(systemPath);
    outProjectId = generateProjectId();

    if (!QDir().mkpath(systemPath)) {
        return FsResult::IoError;
    }

    QFile meta(toSystemPath(systemPath + "/.ourpaint"));

    if (meta.exists()) {
        return FsResult::AlreadyExists;
    }

    if (!meta.open(QIODevice::WriteOnly)) {
        return FsResult::IoError;
    }

    QXmlStreamWriter xml(&meta);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("project");
    xml.writeTextElement("id", outProjectId);
    xml.writeTextElement("name", name);
    xml.writeStartElement("tabs");
    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndDocument();
    meta.close();

    return addProject(name, toStoragePath(systemPath), outProjectId);
}


UI::FileSystem::FsResult UI::FileSystem::openProjectById(const QString& projectId,
                                                         ProjectData& outData) const {
    auto projects = loadProjects();

    for (const auto& p: projects) {
        if (p.id == projectId) {
            const auto systemPath = toSystemPath(p.path);
            const auto metaFilePath = toSystemPath(systemPath + "/.ourpaint");

            if (!QDir(systemPath).exists()) {
                return FsResult::NotFound;
            }

            if (!QFile::exists(metaFilePath)) {
                return FsResult::NotFound;
            }

            QFile meta(metaFilePath);
            if (!meta.open(QIODevice::ReadOnly)) {
                return FsResult::IoError;
            }

            QXmlStreamReader xml(&meta);
            QString metaId;

            while (!xml.atEnd()) {
                xml.readNext();
                if (xml.isStartElement() && xml.name() == "id") {
                    metaId = xml.readElementText();
                }
            }

            meta.close();

            if (xml.hasError()) {
                return FsResult::IoError;
            }

            if (metaId.isEmpty()) {
                metaId = projectId;
            }

            const auto realName = extractProjectNameFromPath(systemPath);

            {
                QFile metaWrite(metaFilePath);
                if (metaWrite.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                    QXmlStreamWriter writer(&metaWrite);
                    writer.setAutoFormatting(true);
                    writer.writeStartDocument();
                    writer.writeStartElement("project");
                    writer.writeTextElement("id", metaId);
                    writer.writeTextElement("name", realName);
                    writer.writeStartElement("tabs");
                    writer.writeEndElement();
                    writer.writeEndElement();
                    writer.writeEndDocument();
                    metaWrite.close();
                }
            }

            renameProjectInXml(metaId,
                               toStoragePath(systemPath),
                               realName);

            outData.tabs.clear();

            const QDir dir(systemPath);
            auto files = dir.entryInfoList(QStringList() << "*.ourp", QDir::Files);

            for (const auto& f: files) {
                outData.tabs.append(f.completeBaseName());
            }

            outData.id = metaId;
            outData.name = realName;
            outData.path = toDisplayPath(systemPath);

            return FsResult::Ok;
        }
    }

    return FsResult::NotFound;
}


UI::FileSystem::FsResult UI::FileSystem::openProjectByPath(const QString& fullPath,
                                                           ProjectData& outData) const {
    const auto systemPath = toSystemPath(fullPath);
    const auto metaFilePath = toSystemPath(systemPath + "/.ourpaint");

    if (!QDir(systemPath).exists()) {
        return FsResult::NotFound;
    }

    if (!QFile::exists(metaFilePath)) {
        return FsResult::NotFound;
    }

    QFile meta(metaFilePath);
    if (!meta.open(QIODevice::ReadOnly)) {
        return FsResult::IoError;
    }

    QXmlStreamReader xml(&meta);

    QString projectId;

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement() && xml.name() == "id") {
            projectId = xml.readElementText();
        }
    }

    meta.close();

    if (xml.hasError()) {
        return FsResult::IoError;
    }

    if (projectId.isEmpty()) {
        projectId = generateProjectId();
    }

    const auto realName = extractProjectNameFromPath(systemPath);

    {
        QFile metaWrite(metaFilePath);
        if (metaWrite.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QXmlStreamWriter writer(&metaWrite);
            writer.setAutoFormatting(true);
            writer.writeStartDocument();
            writer.writeStartElement("project");
            writer.writeTextElement("id", projectId);
            writer.writeTextElement("name", realName);
            writer.writeStartElement("tabs");
            writer.writeEndElement();
            writer.writeEndElement();
            writer.writeEndDocument();
            metaWrite.close();
        }
    }

    auto projects = loadProjects();
    bool found = false;

    QVector<ProjectData> updated;
    const auto comparablePath = toComparablePath(systemPath);

    for (const auto& p: projects) {
        if (toComparablePath(p.path) == comparablePath) {
            auto upd = p;
            upd.name = realName;
            upd.path = toStoragePath(systemPath);
            updated.append(upd);
            found = true;
        } else {
            updated.append(p);
        }
    }

    if (!found) {
        QFile file(projectsXml_);
        if (!file.open(QIODevice::ReadOnly)) {
            return FsResult::IoError;
        }

        QXmlStreamReader xmlReader(&file);
        QVector<ProjectData> existingProjects;

        while (!xmlReader.atEnd()) {
            xmlReader.readNext();
            if (xmlReader.isStartElement() && xmlReader.name() == "project") {
                ProjectData p;
                p.id = xmlReader.attributes().value("id").toString();
                p.name = xmlReader.attributes().value("name").toString();
                p.path = xmlReader.attributes().value("path").toString();
                existingProjects.append(p);
            }
        }

        file.close();

        if (xmlReader.hasError()) {
            return FsResult::IoError;
        }

        ProjectData newProj;
        newProj.id = projectId;
        newProj.name = realName;
        newProj.path = toStoragePath(systemPath);
        existingProjects.append(newProj);

        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            return FsResult::IoError;
        }

        QXmlStreamWriter xmlWriter(&file);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("projects");

        for (const auto& p : existingProjects) {
            xmlWriter.writeStartElement("project");
            xmlWriter.writeAttribute("id", p.id);
            xmlWriter.writeAttribute("name", p.name);
            xmlWriter.writeAttribute("path", p.path);
            xmlWriter.writeEndElement();
        }

        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
        file.close();
    } else {
        QFile file(projectsXml_);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            return FsResult::IoError;
        }

        QXmlStreamWriter xml(&file);
        xml.setAutoFormatting(true);
        xml.writeStartDocument();
        xml.writeStartElement("projects");

        for (const auto& p: updated) {
            xml.writeStartElement("project");
            xml.writeAttribute("id", p.id);
            xml.writeAttribute("name", p.name);
            xml.writeAttribute("path", toStoragePath(p.path));
            xml.writeEndElement();
        }

        xml.writeEndElement();
        xml.writeEndDocument();
        file.close();
    }

    outData.tabs.clear();

    const QDir dir(systemPath);
    auto files = dir.entryInfoList(QStringList() << "*.ourp", QDir::Files);

    for (const auto& f: files) {
        outData.tabs.append(f.completeBaseName());
    }

    outData.id = projectId;
    outData.name = realName;
    outData.path = toDisplayPath(systemPath);

    return FsResult::Ok;
}


UI::FileSystem::FsResult UI::FileSystem::openFile(const QString& filePath, FileData& outData) const {
    const auto systemFilePath = toSystemPath(filePath);
    const QFileInfo fileInfo(systemFilePath);

    if (!fileInfo.exists()) {
        return FsResult::NotFound;
    }

    if (fileInfo.suffix() != "ourp") {
        return FsResult::NotFound;
    }

    const auto projectPath = fileInfo.absolutePath();
    const auto comparableProjectPath = toComparablePath(projectPath);

    auto projects = loadProjects();
    QString projectId;

    for (const auto& p: projects) {
        if (auto comparableXmlPath = toComparablePath(p.path); comparableXmlPath == comparableProjectPath) {
            projectId = p.id;
            break;
        }
    }

    if (projectId.isEmpty()) {
        return FsResult::NotFound;
    }

    outData.tabName = fileInfo.completeBaseName();
    outData.projectId = projectId;
    outData.projectPath = toDisplayPath(projectPath);

    return FsResult::Ok;
}


UI::FileSystem::FsResult UI::FileSystem::removeProjectById(const QString& projectId) const {
    auto projects = loadProjectsRaw();
    QString path;

    for (const auto& p: projects) {
        if (p.id == projectId) {
            path = p.path;
            break;
        }
    }

    if (path.isEmpty()) {
        return FsResult::NotFound;
    }

    const auto systemPath = toSystemPath(path);
    if (QDir dir(systemPath); !dir.removeRecursively()) {
        return FsResult::IoError;
    }

    return removeProjectFromXml(projectId);
}


UI::FileSystem::FsResult UI::FileSystem::removeProjectFromXml(const QString& projectId) const {
    auto projects = loadProjectsRaw();

    QVector<ProjectData> updated;
    bool found = false;

    for (const auto& p : projects) {
        if (p.id == projectId) {
            found = true;
            continue;
        }

        updated.append(p);
    }

    if (!found) {
        return FsResult::NotFound;
    }

    saveProjects(updated);
    return FsResult::Ok;
}


UI::FileSystem::FsResult UI::FileSystem::renameProjectById(const QString& projectId,
                                                           const QString& newName,
                                                           ProjectData& outData) const {
    if (newName.isEmpty() || newName.contains('/') || newName.contains('\\')) {
        return FsResult::IoError;
    }

    auto projects = loadProjects();
    QString oldPath;

    for (const auto& p: projects) {
        if (p.id == projectId) {
            oldPath = p.path;
            break;
        }
    }

    if (oldPath.isEmpty()) {
        return FsResult::NotFound;
    }

    const auto oldSystemPath = toSystemPath(oldPath);
    const auto baseDir = QFileInfo(oldSystemPath).absolutePath();
    const auto newSystemPath = toSystemPath(baseDir + "/" + newName);

    if (QDir(newSystemPath).exists()) {
        return FsResult::AlreadyExists;
    }

    if (!QDir().rename(oldSystemPath, newSystemPath)) {
        return FsResult::IoError;
    }

    QFile meta(toSystemPath(newSystemPath + "/.ourpaint"));
    if (meta.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QXmlStreamWriter writer(&meta);
        writer.setAutoFormatting(true);
        writer.writeStartDocument();
        writer.writeStartElement("project");
        writer.writeTextElement("id", projectId);
        writer.writeTextElement("name", newName);
        writer.writeStartElement("tabs");
        writer.writeEndElement();
        writer.writeEndElement();
        writer.writeEndDocument();
        meta.close();
    }

    const auto res = renameProjectInXml(projectId,
                                        toStoragePath(newSystemPath),
                                        newName);
    if (res != FsResult::Ok) {
        return res;
    }

    outData.tabs.clear();

    const QDir dir(newSystemPath);
    auto files = dir.entryInfoList(QStringList() << "*.ourp", QDir::Files);

    for (const auto& f: files) {
        outData.tabs.append(f.completeBaseName());
    }

    outData.id = projectId;
    outData.name = newName;
    outData.path = toDisplayPath(newSystemPath);

    return FsResult::Ok;
}


UI::FileSystem::FsResult UI::FileSystem::renameProjectByPath(const QString& fullPath,
                                                             const QString& newName,
                                                             ProjectData& outData) const {
    if (newName.isEmpty() || newName.contains('/') || newName.contains('\\')) {
        return FsResult::IoError;
    }

    const auto oldSystemPath = toSystemPath(fullPath);

    if (!QDir(oldSystemPath).exists()) {
        return FsResult::NotFound;
    }

    const auto baseDir = QFileInfo(oldSystemPath).absolutePath();
    const auto newSystemPath = toSystemPath(baseDir + "/" + newName);

    if (QDir(newSystemPath).exists()) {
        return FsResult::AlreadyExists;
    }

    QString projectId;
    QFile oldMeta(toSystemPath(oldSystemPath + "/.ourpaint"));

    if (oldMeta.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xml(&oldMeta);

        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement() && xml.name() == "id") {
                projectId = xml.readElementText();
            }
        }

        oldMeta.close();
    }

    if (projectId.isEmpty()) {
        projectId = generateProjectId();
    }

    if (!QDir().rename(oldSystemPath, newSystemPath)) {
        return FsResult::IoError;
    }

    QFile meta(toSystemPath(newSystemPath + "/.ourpaint"));
    if (meta.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QXmlStreamWriter writer(&meta);
        writer.setAutoFormatting(true);
        writer.writeStartDocument();
        writer.writeStartElement("project");
        writer.writeTextElement("id", projectId);
        writer.writeTextElement("name", newName);
        writer.writeStartElement("tabs");
        writer.writeEndElement();
        writer.writeEndElement();
        writer.writeEndDocument();
        meta.close();
    }

    auto projects = loadProjects();
    bool found = false;

    QVector<ProjectData> updated;
    const auto oldComparable = toComparablePath(oldSystemPath);

    for (const auto& p: projects) {
        if (toComparablePath(p.path) == oldComparable) {
            auto upd = p;
            upd.name = newName;
            upd.path = toStoragePath(newSystemPath);
            updated.append(upd);
            found = true;
        } else {
            updated.append(p);
        }
    }

    if (!found) {
        const auto res = addProject(newName, toStoragePath(newSystemPath), projectId);
        if (res != FsResult::Ok) {
            return res;
        }
    } else {
        QFile file(projectsXml_);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            return FsResult::IoError;
        }

        QXmlStreamWriter xml(&file);
        xml.setAutoFormatting(true);
        xml.writeStartDocument();
        xml.writeStartElement("projects");

        for (const auto& p: updated) {
            xml.writeStartElement("project");
            xml.writeAttribute("id", p.id);
            xml.writeAttribute("name", p.name);
            xml.writeAttribute("path", toStoragePath(p.path));
            xml.writeEndElement();
        }

        xml.writeEndElement();
        xml.writeEndDocument();
        file.close();
    }

    // --- СОБИРАЕМ DATA ---
    outData.tabs.clear();

    const QDir dir(newSystemPath);
    auto files = dir.entryInfoList(QStringList() << "*.ourp", QDir::Files);

    for (const auto& f: files) {
        outData.tabs.append(f.completeBaseName());
    }

    outData.id = projectId;
    outData.name = newName;
    outData.path = toDisplayPath(newSystemPath);

    return FsResult::Ok;
}


UI::FileSystem::FsResult UI::FileSystem::createTab(const QString& projectId, const QString& tabName) const {
    if (tabName.isEmpty()) {
        return FsResult::IoError;
    }

    ProjectData pd;

    if (const auto result = openProjectById(projectId, pd); result != FsResult::Ok) {
        return result;
    }

    const auto systemPath = toSystemPath(pd.path);
    const auto filePath = toSystemPath(systemPath + "/" + tabName + ".ourp");

    if (QFile::exists(filePath)) {
        return FsResult::AlreadyExists;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return FsResult::IoError;
    }

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("tab");
    xml.writeAttribute("name", tabName);
    xml.writeEndElement();
    xml.writeEndDocument();

    file.close();
    return FsResult::Ok;
}


UI::FileSystem::FsResult UI::FileSystem::removeTab(const QString& projectId, const QString& tabName) const {
    ProjectData pd;

    if (const auto result = openProjectById(projectId, pd); result != FsResult::Ok) {
        return result;
    }

    const auto systemPath = toSystemPath(pd.path);
    const auto filePath = toSystemPath(systemPath + "/" + tabName + ".ourp");

    if (!QFile::exists(filePath)) {
        return FsResult::NotFound;
    }

    if (!QFile::remove(filePath)) {
        return FsResult::IoError;
    }

    return FsResult::Ok;
}


UI::FileSystem::FsResult UI::FileSystem::renameTab(const QString& projectId,
                                                   const QString& oldName,
                                                   const QString& newName) const {
    if (oldName.isEmpty() || newName.isEmpty()) {
        return FsResult::IoError;
    }

    if (oldName == newName) {
        return FsResult::Ok;
    }

    ProjectData pd;

    if (const auto result = openProjectById(projectId, pd); result != FsResult::Ok) {
        return result;
    }

    const auto systemPath = toSystemPath(pd.path);
    const auto oldPath = toSystemPath(systemPath + "/" + oldName + ".ourp");
    const auto newPath = toSystemPath(systemPath + "/" + newName + ".ourp");

    if (!QFile::exists(oldPath)) {
        return FsResult::NotFound;
    }

    if (QFile::exists(newPath)) {
        return FsResult::AlreadyExists;
    }

    if (!QFile::rename(oldPath, newPath)) {
        return FsResult::IoError;
    }

    return FsResult::Ok;
}


UI::FileSystem::FsResult UI::FileSystem::addProject(const QString& name, const QString& path,
                                                    const QString& outId) const {
    auto projects = loadProjects();

    for (const auto& p: projects) {
        if (p.id == outId) {
            return FsResult::AlreadyExists;
        }
    }

    QFile file(projectsXml_);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return FsResult::IoError;
    }

    QXmlStreamWriter writer(&file);
    writer.setAutoFormatting(true);
    writer.writeStartDocument();
    writer.writeStartElement("projects");

    for (const auto& p: projects) {
        writer.writeStartElement("project");
        writer.writeAttribute("id", p.id);
        writer.writeAttribute("name", p.name);
        writer.writeAttribute("path", toStoragePath(p.path));
        writer.writeEndElement();
    }

    writer.writeStartElement("project");
    writer.writeAttribute("id", outId);
    writer.writeAttribute("name", name);
    writer.writeAttribute("path", toStoragePath(path));
    writer.writeEndElement();

    writer.writeEndElement();
    writer.writeEndDocument();
    file.close();

    return FsResult::Ok;
}


QVector<UI::FileSystem::ProjectData> UI::FileSystem::loadProjectsRaw() const {
    QVector<ProjectData> list;
    QFile file(projectsXml_);

    if (!file.open(QIODevice::ReadOnly)) {
        return list;
    }

    QXmlStreamReader xml(&file);

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement() && xml.name() == "project") {
            ProjectData p;
            p.id = xml.attributes().value("id").toString();
            p.name = xml.attributes().value("name").toString();
            p.path = toComparablePath(xml.attributes().value("path").toString());

            if (!p.id.isEmpty() && !p.path.isEmpty()) {
                list.append(p);
            }
        }
    }

    return list;
}


QVector<UI::FileSystem::ProjectData> UI::FileSystem::loadProjects() const {
    const auto raw = loadProjectsRaw();

    QVector<ProjectData> clean;
    bool changed = false;

    for (const auto& p : raw) {
        const QString systemPath = toSystemPath(p.path);

        if (QDir(systemPath).exists()) {
            clean.append(p);
        } else {
            changed = true;
        }
    }

    if (changed) {
        saveProjects(clean);
    }

    return clean;
}


void UI::FileSystem::saveProjects(const QVector<ProjectData>& projects) const {
    QFile file(projectsXml_);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return;
    }

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);

    xml.writeStartDocument();
    xml.writeStartElement("projects");

    for (const auto& p : projects) {
        xml.writeStartElement("project");
        xml.writeAttribute("id", p.id);
        xml.writeAttribute("name", p.name);
        xml.writeAttribute("path", toStoragePath(p.path));
        xml.writeEndElement();
    }

    xml.writeEndElement();
    xml.writeEndDocument();
}


UI::FileSystem::FsResult UI::FileSystem::renameProjectInXml(const QString& projectId,
                                                            const QString& newPath,
                                                            const QString& newName) const {
    auto projects = loadProjects();
    QVector<ProjectData> updatedList;
    bool found = false;

    for (const auto& p: projects) {
        if (p.id == projectId) {
            auto updated = p;
            updated.path = newPath;
            updated.name = newName;
            updatedList.append(updated);
            found = true;
        } else {
            updatedList.append(p);
        }
    }

    if (!found) {
        return FsResult::NotFound;
    }

    QFile file(projectsXml_);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return FsResult::IoError;
    }

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    xml.writeStartElement("projects");

    for (const auto& p: updatedList) {
        xml.writeStartElement("project");
        xml.writeAttribute("id", p.id);
        xml.writeAttribute("name", p.name);
        xml.writeAttribute("path", toStoragePath(p.path));
        xml.writeEndElement();
    }

    xml.writeEndElement();
    xml.writeEndDocument();
    file.close();

    return FsResult::Ok;
}


void UI::FileSystem::initialize() const {
    QDir().mkpath(projectsDir_);
    QDir().mkpath(settingsDir_);

    if (!QFile::exists(projectsXml_)) {
        QFile file(projectsXml_);
        if (file.open(QIODevice::WriteOnly)) {
            QXmlStreamWriter xml(&file);
            xml.setAutoFormatting(true);
            xml.writeStartDocument();
            xml.writeStartElement("projects");
            xml.writeEndElement();
            xml.writeEndDocument();
            file.close();
        }
    }
}


QString UI::FileSystem::extractProjectNameFromPath(const QString& fullPath) {
    const QDir dir(fullPath);
    return dir.dirName();
}


QString UI::FileSystem::generateProjectId() {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}
