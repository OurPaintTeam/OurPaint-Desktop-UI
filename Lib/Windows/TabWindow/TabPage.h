#ifndef OURPAINT_DESKTOP_UI_TABPAGE_H
#define OURPAINT_DESKTOP_UI_TABPAGE_H

#include "../BaseEditorPage.h"

namespace UI {
    class SideTool;
    class TopBarTab;


    class TabPage final : public BaseEditorPage {
        Q_OBJECT

    public:
        explicit TabPage(QWidget* parent = nullptr);
        ~TabPage() override = default;

    private:
        void initUI() override;

        TopBarTab* topBar_{nullptr};
        SideTool* leftTool_{nullptr};
        SideTool* rightTool_{nullptr};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_TABPAGE_H
