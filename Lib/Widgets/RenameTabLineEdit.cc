#include "RenameTabLineEdit.h"

UI::RenameTabLineEdit::RenameTabLineEdit(QWidget* parent): QLineEdit(parent) {
}

void UI::RenameTabLineEdit::focusOutEvent(QFocusEvent* e)
{
    QLineEdit::focusOutEvent(e);
    emit focusLost();
}
