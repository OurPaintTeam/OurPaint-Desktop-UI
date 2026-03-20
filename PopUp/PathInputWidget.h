#ifndef OURPAINT_DESKTOP_UI_PATHINPUTWIDGET_H
#define OURPAINT_DESKTOP_UI_PATHINPUTWIDGET_H
#include "InputWidget.h"


class QCheckBox;

namespace UI {
    class PathInputWidget final : public InputWidget {
        Q_OBJECT

    public:
        explicit PathInputWidget(const QString& promptText,
                                 QWidget* parent = nullptr);

        void setCheckBoxQuestion(const QString& text);
        bool isCheckBoxChecked() const;

    private:
        QWidget* parentWidget_{nullptr};
        QCheckBox* checkBox_{nullptr};
        QPushButton* browseButton_{nullptr};
        QHBoxLayout* layout_{nullptr};

    private slots:
        void openFolderSlot() ;
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_PATHINPUTWIDGET_H
