#include "RenameTabLineEdit.h"

UI::RenameTabLineEdit::RenameTabLineEdit(QWidget* parent): QLineEdit(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setAutoFillBackground(true);
}

void UI::RenameTabLineEdit::focusOutEvent(QFocusEvent* e)
{
    QLineEdit::focusOutEvent(e);
    emit focusLost();
}


void UI::RenameTabLineEdit::paintEvent(QPaintEvent* e)
{
    QLineEdit::paintEvent(e);
}