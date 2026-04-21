#include "RenameTabLineEdit.h"

UI::RenameTabLineEdit::RenameTabLineEdit(QWidget* parent): QLineEdit(parent) {
  setAttribute(Qt::WA_StyledBackground, true);
  setAutoFillBackground(false);
  setAttribute(Qt::WA_TranslucentBackground, true);
}

void UI::RenameTabLineEdit::focusOutEvent(QFocusEvent* e)
{
    QLineEdit::focusOutEvent(e);
    emit focusLost();
}