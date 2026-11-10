#include "PathInputWidget.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QIcon>
#include <QLineEdit>
#include <QPushButton>


UI::PathInputWidget::PathInputWidget(const QString& promptText,
                                     QWidget* parent,
                                     const PathMode mode)
    : InputWidget(promptText, parent),
      parentWidget_(parent),
      browseButton_(new QPushButton(this)),
      layout_(new QHBoxLayout()),
      mode_(mode) {
    setObjectName("PathInputWidget");

    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(4);

    auto* edit = lineEdit();
    mainLayout()->removeWidget(edit);

    browseButton_->setObjectName("PathInputWidgetBrowseButton");
    browseButton_->setFixedSize(QSize(26, 26));
    browseButton_->setIconSize(QSize(16, 16));

    layout_->addWidget(edit);
    layout_->addWidget(browseButton_);
    mainLayout()->insertLayout(1, layout_);

    updateBrowseButton();

    connect(browseButton_, &QPushButton::clicked,
            this, &PathInputWidget::browseSlot);
}


void UI::PathInputWidget::setMode(const PathMode mode) {
    if (mode_ == mode) return;
    mode_ = mode;
    updateBrowseButton();
}


void UI::PathInputWidget::updateBrowseButton() {
    switch (mode_) {
    case PathMode::Folder:
        browseButton_->setIcon(QIcon(":/Assets/icons/folder/openFileLight.svg"));
        browseButton_->setToolTip(tr("Select folder"));
        break;
    case PathMode::Txt:
    case PathMode::Ourp:
    case PathMode::All:
        // подставьте иконку файла, которая есть в проекте
        browseButton_->setIcon(QIcon(":/Assets/icons/folder/openFileLight.svg"));
        browseButton_->setToolTip(tr("Select file"));
        break;
    }
}


void UI::PathInputWidget::browseSlot() {
    hide();

    QString result;
    switch (mode_) {
    case PathMode::Folder:
        result = QFileDialog::getExistingDirectory(
            parentWidget_,
            tr("Select Folder"),
            lineEdit()->text(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        break;

    case PathMode::Txt:
        result = QFileDialog::getOpenFileName(
            parentWidget_,
            tr("Select TXT File"),
            lineEdit()->text(),
            tr("Text Files") + " (*.txt);;" + tr("All Files") + " (*)");
        break;

    case PathMode::Ourp:
        result = QFileDialog::getOpenFileName(
            parentWidget_,
            tr("Select OurPaint File"),
            lineEdit()->text(),
            tr("OurPaint Files") + " (*.ourp);;" + tr("All Files") + " (*)");
        break;

    case PathMode::All:
        result = QFileDialog::getOpenFileName(
            parentWidget_,
            tr("Select File"),
            lineEdit()->text(),
            tr("All Files") + " (*)");
        break;
    }

    show();

    if (!result.isEmpty()) {
        lineEdit()->setText(result);
    }
}

