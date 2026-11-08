#include "SettingsSystem.h"

#include <QDir>
#include <QMutexLocker>
#include <cmath>

namespace UI {

namespace {

QString languageToCode(SettingsSystem::Language language) {
    switch (language) {
    case SettingsSystem::Language::Russian:    return QStringLiteral("ru");
    case SettingsSystem::Language::Belarusian: return QStringLiteral("be");
    case SettingsSystem::Language::English:
    default:                                   return QStringLiteral("en");
    }
}

SettingsSystem::Language languageFromCode(const QString& code) {
    if (code.compare(QStringLiteral("ru"), Qt::CaseInsensitive) == 0)
        return SettingsSystem::Language::Russian;
    if (code.compare(QStringLiteral("be"), Qt::CaseInsensitive) == 0)
        return SettingsSystem::Language::Belarusian;
    return SettingsSystem::Language::English;
}

bool fuzzyEq(double a, double b) {
    if (a == b) return true;
    return std::abs(a - b) < 1e-9;
}

} // namespace

SettingsSystem::SettingsSystem(const QString& settingsDirectory)
    : settings_(QDir(settingsDirectory).filePath(QStringLiteral("settings.ini")),
                QSettings::IniFormat)
{
    reload();
}

SettingsSystem::AllSettings SettingsSystem::loadAll()
{
    QMutexLocker lock(&mutex_);

    AllSettings all;
    all.sketch   = sketchSettingsLocked();
    all.language = languageLocked();

    cachedSketch_   = all.sketch;
    cachedLanguage_ = all.language;
    cacheValid_     = true;

    return all;
}

SettingsSystem::SettingsDiff SettingsSystem::loadChanges()
{
    QMutexLocker lock(&mutex_);

    const SketchSettings nowSketch   = sketchSettingsLocked();
    const Language       nowLanguage = languageLocked();

    SettingsDiff d;

    if (!cacheValid_) {
        d.sketch.gridEnabled = nowSketch.gridEnabled;
        d.sketch.snapEnabled = nowSketch.snapEnabled;
        d.sketch.gridStep    = nowSketch.gridStep;
        d.sketch.snapStep    = nowSketch.snapStep;
        d.language           = nowLanguage;
    } else {
        if (nowSketch.gridEnabled != cachedSketch_.gridEnabled)
            d.sketch.gridEnabled = nowSketch.gridEnabled;
        if (nowSketch.snapEnabled != cachedSketch_.snapEnabled)
            d.sketch.snapEnabled = nowSketch.snapEnabled;
        if (!fuzzyEq(nowSketch.gridStep, cachedSketch_.gridStep))
            d.sketch.gridStep = nowSketch.gridStep;
        if (!fuzzyEq(nowSketch.snapStep, cachedSketch_.snapStep))
            d.sketch.snapStep = nowSketch.snapStep;
        if (nowLanguage != cachedLanguage_)
            d.language = nowLanguage;
    }

    cachedSketch_   = nowSketch;
    cachedLanguage_ = nowLanguage;
    cacheValid_     = true;

    return d;
}

SettingsSystem::SketchSettings SettingsSystem::sketchSettings() const
{
    QMutexLocker lock(&mutex_);
    return sketchSettingsLocked();
}

void SettingsSystem::setSketchSettings(const SketchSettings& settings)
{
    QMutexLocker lock(&mutex_);
    settings_.beginGroup(QStringLiteral("Sketch"));
    settings_.setValue(QStringLiteral("GridEnabled"), settings.gridEnabled);
    settings_.setValue(QStringLiteral("SnapEnabled"), settings.snapEnabled);
    settings_.setValue(QStringLiteral("GridStep"),    settings.gridStep);
    settings_.setValue(QStringLiteral("SnapStep"),    settings.snapStep);
    settings_.endGroup();
}

bool SettingsSystem::isGridEnabled() const
{
    QMutexLocker lock(&mutex_);
    return settings_.value(QStringLiteral("Sketch/GridEnabled"), true).toBool();
}

void SettingsSystem::setGridEnabled(bool enabled)
{
    QMutexLocker lock(&mutex_);
    settings_.setValue(QStringLiteral("Sketch/GridEnabled"), enabled);
}

bool SettingsSystem::isSnapEnabled() const
{
    QMutexLocker lock(&mutex_);
    return settings_.value(QStringLiteral("Sketch/SnapEnabled"), true).toBool();
}

void SettingsSystem::setSnapEnabled(bool enabled)
{
    QMutexLocker lock(&mutex_);
    settings_.setValue(QStringLiteral("Sketch/SnapEnabled"), enabled);
}

double SettingsSystem::gridStep() const
{
    QMutexLocker lock(&mutex_);
    return settings_.value(QStringLiteral("Sketch/GridStep"), 10.0).toDouble();
}

void SettingsSystem::setGridStep(double step)
{
    QMutexLocker lock(&mutex_);
    settings_.setValue(QStringLiteral("Sketch/GridStep"), step);
}

double SettingsSystem::snapStep() const
{
    QMutexLocker lock(&mutex_);
    return settings_.value(QStringLiteral("Sketch/SnapStep"), 5.0).toDouble();
}

void SettingsSystem::setSnapStep(double step)
{
    QMutexLocker lock(&mutex_);
    settings_.setValue(QStringLiteral("Sketch/SnapStep"), step);
}

SettingsSystem::Language SettingsSystem::language() const
{
    QMutexLocker lock(&mutex_);
    return languageLocked();
}

void SettingsSystem::setLanguage(Language language)
{
    QMutexLocker lock(&mutex_);
    settings_.setValue(QStringLiteral("General/Language"),
                       languageToCode(language));
}

void SettingsSystem::reload()
{
    QMutexLocker lock(&mutex_);
    settings_.sync();
    cacheValid_ = false;
}

void SettingsSystem::save()
{
    QMutexLocker lock(&mutex_);
    settings_.sync();
}

SettingsSystem::SketchSettings SettingsSystem::sketchSettingsLocked() const
{
    SketchSettings r;
    r.gridEnabled = settings_.value(QStringLiteral("Sketch/GridEnabled"), true).toBool();
    r.snapEnabled = settings_.value(QStringLiteral("Sketch/SnapEnabled"), true).toBool();
    r.gridStep    = settings_.value(QStringLiteral("Sketch/GridStep"),   10.0).toDouble();
    r.snapStep    = settings_.value(QStringLiteral("Sketch/SnapStep"),    5.0).toDouble();
    return r;
}

SettingsSystem::Language SettingsSystem::languageLocked() const
{
    const QString code = settings_
        .value(QStringLiteral("General/Language"), QStringLiteral("en"))
        .toString();
    return languageFromCode(code);
}

} // namespace UI