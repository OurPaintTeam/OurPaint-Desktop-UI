#ifndef OURPAINT_DESKTOP_UI_TOPBARTAB_H
#define OURPAINT_DESKTOP_UI_TOPBARTAB_H

#include <QLabel>
#include <QString>
#include <QPushButton>

#include "BaseEditorTopBar.h"

namespace UI {
    class SideTool;
    class SettingsSystem;
    class SettingsWidget;

    // Top bar for detached Tab
    class TopBarLightEditor final : public BaseEditorTopBar {
        Q_OBJECT

    public:
        explicit TopBarLightEditor(QWidget *parent = nullptr);
        ~TopBarLightEditor() override = default;

        void setLeftTool(SideTool *tool);
        void setRightTool(SideTool *tool);
        void setConsole(QWidget *console);
        void setDisplayName(const QString& name);
        void setSettings(SettingsSystem *settings);

        signals:
            void returnLightEditorWindowTriggered();
            void settingsChanged();

    protected:
        void changeEvent(QEvent *e) override;

    private:
        QLabel *title_{nullptr};
        QPushButton *returnBtn_{nullptr};
        QPushButton *btnSettings_{nullptr};
        SettingsWidget *settingsWidget_{nullptr};
        SettingsSystem *settingsSystem_{nullptr};
        QString displayName_;

        void createLightEditorWidgets();
        void setupSettings();
        void updateTitle() const;
        void translate() const;
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_TOPBARTAB_H