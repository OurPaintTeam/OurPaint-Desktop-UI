#ifndef OURPAINT_DESKTOP_UI_PATHINPUTWIDGET_H
#define OURPAINT_DESKTOP_UI_PATHINPUTWIDGET_H
#include "InputWidget.h"

class QCheckBox;
class QPushButton;
class QHBoxLayout;

namespace UI {

    enum class PathMode {
        Folder,
        Txt,
        Ourp,
        All,
    };

    class PathInputWidget final : public InputWidget {
        Q_OBJECT

    public:
        explicit PathInputWidget(const QString& promptText,
                                 QWidget* parent = nullptr,
                                 PathMode mode = PathMode::Folder);

        void setMode(PathMode mode);

    private:
        void updateBrowseButton();
        void browseSlot();

        QWidget*      parentWidget_{nullptr};
        QCheckBox*    checkBox_{nullptr};
        QPushButton*  browseButton_{nullptr};
        QHBoxLayout*  layout_{nullptr};
        PathMode      mode_{PathMode::Folder};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_PATHINPUTWIDGET_H