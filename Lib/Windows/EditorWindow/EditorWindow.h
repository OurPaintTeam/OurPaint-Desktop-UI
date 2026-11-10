#ifndef OURPAINT_DESKTOP_UI_PROJECTWINDOW_H
#define OURPAINT_DESKTOP_UI_PROJECTWINDOW_H

#include <QString>

#include "BaseEditorWindow.h"
#include "Utils.h"

class QCloseEvent;

namespace UI {
    class SettingsSystem;
    class EditorPage;


    class EditorWindow : public BaseEditorWindow {
        Q_OBJECT

    public:
        explicit EditorWindow(QWidget* parent = nullptr);
        ~EditorWindow() override = default;

        void initUI() override;
        void setDefaultPath(const QString& path);
        void addTabSlot(const FileData& data) const;
        void deleteTabSlot(const qint64 id) const;
        void renameTabSlot(FileData& data) const;

    signals:
        void closeApplicationTriggered();

        void goToStartWindowTriggered();

        void createFileTriggered(const QString& path,bool newWindow);
        void openFileTriggered(const QString& path,bool newWindow);

        void renameTabTriggered(FileData& data);
        void removeTabTriggered(const qint64 id);
        void setActiveTabTriggered(const qint64 id);
        void closeTabTriggered(const qint64 id);

        void openLightEditorWindowTriggered(const qint64 id);
        void onOpenScriptTriggered(const QString& path);

         void settingsChanged();

    protected:
        void closeEvent(QCloseEvent* event) override;
        void applyLanguage(UI::SettingsSystem::Language lang);

    private:
        EditorPage* editorPage_{nullptr};
        SettingsSystem* settings_{nullptr};
        qint64 id_{-1};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_PROJECTWINDOW_H
