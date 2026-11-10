#ifndef OURPAINT_SETTINGSSYSTEM_H
#define OURPAINT_SETTINGSSYSTEM_H

#include <QMutex>
#include <QSettings>
#include <QString>
#include <optional>

namespace UI {

    class SettingsSystem {
    public:
        enum class Language { English, Russian, Belarusian };

        struct SketchSettings {
            bool gridEnabled = true;
            bool snapEnabled = true;
            double gridStep = 10.0;
            double snapStep = 5.0;
        };

        struct SketchSettingsDiff {
            std::optional<bool>   gridEnabled;
            std::optional<bool>   snapEnabled;
            std::optional<double> gridStep;
            std::optional<double> snapStep;

            [[nodiscard]] bool isEmpty() const {
                return !gridEnabled && !snapEnabled && !gridStep && !snapStep;
            }
        };

        struct SettingsDiff {
            SketchSettingsDiff sketch;
            std::optional<Language> language;

            [[nodiscard]] bool isEmpty() const {
                return sketch.isEmpty() && !language.has_value();
            }
        };

        struct AllSettings {
            SketchSettings sketch;
            Language language = Language::English;
        };

        explicit SettingsSystem(const QString& settingsDirectory);

        AllSettings loadAll();
        SettingsDiff loadChanges();

        SketchSettings sketchSettings() const;
        void setSketchSettings(const SketchSettings& settings);

        bool isGridEnabled() const;
        void setGridEnabled(bool enabled);

        bool isSnapEnabled() const;
        void setSnapEnabled(bool enabled);

        double gridStep() const;
        void setGridStep(double step);

        double snapStep() const;
        void setSnapStep(double step);

        bool isNotificationsEnabled() const;
        void setNotificationsEnabled(bool enabled);

        Language language() const;
        void setLanguage(Language language);

        void reload();
        void save();

    private:
        mutable QMutex mutex_;
        QSettings settings_;

        SketchSettings cachedSketch_;
        Language cachedLanguage_ = Language::English;
        bool cacheValid_ = false;

        SketchSettings sketchSettingsLocked() const;
        Language languageLocked() const;

    };

} // namespace UI

#endif