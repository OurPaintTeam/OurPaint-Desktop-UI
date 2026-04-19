#include "ConfirmDialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

UI::ConfirmDialog::ConfirmDialog(const QString& questionText,
                                 const QString& yesText,
                                 const QString& noText,
                                 QWidget* parent)
    : InputWidget(questionText, parent) {

    label_->setText(questionText);
    lineEdit_->hide();
    
    if (okButton_) {
        buttonLayout_->removeWidget(okButton_);
        delete okButton_;
        okButton_ = nullptr;
    }
    
    setupButtons(yesText, noText);
    
    setMinimumWidth(250);
    adjustSize();
}

void UI::ConfirmDialog::setupButtons(const QString& yesText, const QString& noText) {
    yesButton_ = new QPushButton(yesText, this);
    yesButton_->setObjectName("ConfirmDialogYesButton");
    yesButton_->setFixedHeight(26);
    
    noButton_ = new QPushButton(noText, this);
    noButton_->setObjectName("ConfirmDialogNoButton");
    noButton_->setFixedHeight(26);
    
    QLayoutItem* item;
    while ((item = buttonLayout_->takeAt(0)) != nullptr) {
        delete item;
    }
    
    buttonLayout_->addStretch();
    buttonLayout_->addWidget(noButton_);
    buttonLayout_->addWidget(yesButton_);
    buttonLayout_->addStretch();
    
    connect(yesButton_, &QPushButton::clicked, this, &ConfirmDialog::onYesClicked);
    connect(noButton_, &QPushButton::clicked, this, &ConfirmDialog::onNoClicked);
}

void UI::ConfirmDialog::setYesButtonText(const QString& text) {
    if (yesButton_) {
        yesButton_->setText(text);
    }
}

void UI::ConfirmDialog::setNoButtonText(const QString& text) {
    if (noButton_) {
        noButton_->setText(text);
    }
}

void UI::ConfirmDialog::onYesClicked() {
    emit yesClicked();
    close();
}

void UI::ConfirmDialog::onNoClicked() {
    emit noClicked();
    close();
}