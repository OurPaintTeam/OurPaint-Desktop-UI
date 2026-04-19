#ifndef OURPAINT_CONFIRMDIALOG_H
#define OURPAINT_CONFIRMDIALOG_H




#include "InputWidget.h"

#include <QPushButton>

namespace UI {
    class ConfirmDialog : public InputWidget {
        Q_OBJECT

    public:
        explicit ConfirmDialog(const QString& questionText,
                              const QString& yesText = "Yes",
                              const QString& noText = "No",
                              QWidget* parent = nullptr);

        void setYesButtonText(const QString& text);
        void setNoButtonText(const QString& text);

        signals:
            void yesClicked();
        void noClicked();

    private slots:
        void onYesClicked();
        void onNoClicked();

    protected:
        void setupButtons(const QString& yesText, const QString& noText);

    private:
        QPushButton* yesButton_{nullptr};
        QPushButton* noButton_{nullptr};
    };
} // namespace UI


#endif //OURPAINT_CONFIRMDIALOG_H
