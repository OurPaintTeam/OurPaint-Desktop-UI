#ifndef OURPAINT_DESKTOP_UI_TABWINDOW_H
#define OURPAINT_DESKTOP_UI_TABWINDOW_H

#include <QString>

#include "BaseEditorWindow.h"
#include "Utils.h"

class QCloseEvent;

namespace UI {
    class LightEditorPage;


    class LightEditorWindow : public BaseEditorWindow {
        Q_OBJECT

    public:
        explicit LightEditorWindow(const FileData& data, QWidget* parent = nullptr);
        ~LightEditorWindow() override = default;

        void initUI() override;
        void setNotificationsEnabled(bool enable);
        qint64 id() const;
        void setName(const QString& name);
        signals:
            void returnTabTriggered(qint64 fileId);

    protected:
        void closeEvent(QCloseEvent* event) override;

    private:
        FileData fileData_;
        LightEditorPage* lightEditorPage_{nullptr};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_TABWINDOW_H