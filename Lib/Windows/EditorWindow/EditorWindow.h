#ifndef OURPAINT_DESKTOP_UI_PROJECTWINDOW_H
#define OURPAINT_DESKTOP_UI_PROJECTWINDOW_H

#include <QString>

#include "BaseEditorWindow.h"

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
        void addTabSlot(const QString& name,const qint64 id) const;
        void deleteTabSlot(const qint64 id) const;
        void renameTabSlot(const qint64 id, const QString& newName) const;

    signals:
        void windowClosedTriggered();
        void goToStartWindowTriggered();
        void setActiveTabTriggered(const qint64 id);
        void openFileThisWindowTriggered(const QString& path);
        void createFileThisWindowTriggered(const QString& path);
        void openNewWindowOpenFileTriggered(const QString& path);
        void openNewWindowCreateFileTriggered(const QString& path);
        void openFileTriggered(const QString& filePath);
        void createFileTriggered(const QString& fileName);
        void renameTabTriggered(const qint64 id, const QString& newName);
        void removeTabTriggered(const qint64 id);
        void openLightEditorWindowTriggered(const qint64 id);
        void onOpenScriptTriggered(const QString& path);
        void closeApplicationTriggered();

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
