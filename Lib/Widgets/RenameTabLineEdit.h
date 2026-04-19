#ifndef OURPAINT_RENAMETABLINEEDIT_H
#define OURPAINT_RENAMETABLINEEDIT_H
#include <QLineEdit>


namespace UI {
    class RenameTabLineEdit final : public QLineEdit
    {
        Q_OBJECT
    public:
        explicit RenameTabLineEdit(QWidget* parent);
        using QLineEdit::QLineEdit;

        signals:
            void focusLost();

    protected:
        void focusOutEvent(QFocusEvent* e) override;
    };
} // namespace UI


#endif //OURPAINT_RENAMETABLINEEDIT_H
