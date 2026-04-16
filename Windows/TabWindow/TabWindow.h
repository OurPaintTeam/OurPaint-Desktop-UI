#ifndef OURPAINT_DESKTOP_UI_TABWINDOW_H
#define OURPAINT_DESKTOP_UI_TABWINDOW_H

#include <QString>

#include "../BaseEditorWindow.h"

class QCloseEvent;

namespace UI {
    class TabPage;


    class TabWindow : public BaseEditorWindow {
        Q_OBJECT

    public:
        explicit TabWindow(const QString& tabName, QWidget* parent = nullptr);
        ~TabWindow() override = default;

        void initUI() override;
        QString tabName() const;
        void setTabName(const QString& tabName);

    signals:
        void returnTabTriggered(const QString& tabName);

    protected:
        void closeEvent(QCloseEvent* event) override;

    private:
        QString tabName_;
        TabPage* tabPage_{nullptr};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_TABWINDOW_H
