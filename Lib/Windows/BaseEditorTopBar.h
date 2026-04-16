#ifndef OURPAINT_DESKTOP_UI_BASEEDITORTOPBAR_H
#define OURPAINT_DESKTOP_UI_BASEEDITORTOPBAR_H

#include <functional>

#include "TopBar.h"

class QPushButton;

namespace UI {
    class BaseEditorTopBar : public TopBar {
        Q_OBJECT

    public:
        explicit BaseEditorTopBar(QWidget* parent = nullptr);
        ~BaseEditorTopBar() override = default;

        void setConsoleButtonEnabled(bool enabled) const;

    protected:
        void setLeftPanel(QWidget* panel);
        void setRightPanel(QWidget* panel);
        void setConsole(QWidget* consoleWidget);
        static void toggleWidgetVisibility(QWidget* widget);

        void setLeftPanel(QWidget* panel, const QIcon& icon, const QString& toolTip);
        void setRightPanel(QWidget* panel, const QIcon& icon, const QString& toolTip);
        void setCommandConsoleWidget(QWidget* consoleWidget);
        QPushButton* createToggleButton(const QIcon& icon,
                                        const QString& toolTip,
                                        const std::function<void()>& onClick);

        QWidget* leftPanel_{nullptr};
        QWidget* rightPanel_{nullptr};
        QWidget* commandConsoleWidget_{nullptr};

        QPushButton* leftButton_{nullptr};
        QPushButton* rightButton_{nullptr};
        QPushButton* consoleButton_{nullptr};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_BASEEDITORTOPBAR_H
