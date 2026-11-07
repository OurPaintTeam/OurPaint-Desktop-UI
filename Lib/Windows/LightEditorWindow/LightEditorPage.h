#ifndef OURPAINT_DESKTOP_UI_TABPAGE_H
#define OURPAINT_DESKTOP_UI_TABPAGE_H

#include "BaseEditorPage.h"

namespace UI {
    class SideTool;
    class TopBarLightEditor;


    class LightEditorPage final : public BaseEditorPage {
        Q_OBJECT

    public:
        explicit LightEditorPage(QWidget* parent = nullptr, const qint64& id = -1, QString  displayName = "");
        void setName(const QString& name);
        ~LightEditorPage() override = default;

        signals:
            void returnLightEditorWindowTriggered();
    private:
        void initUI() override;

        TopBarLightEditor* topBar_{nullptr};
        SideTool* leftTool_{nullptr};
        SideTool* rightTool_{nullptr};
        QString displayName_;
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_TABPAGE_H