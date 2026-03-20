#include "PathInputWidget.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>


UI::PathInputWidget::PathInputWidget(const QString& promptText,
                                     QWidget* parent)
    : InputWidget(promptText, parent),
      parentWidget_(parent),
      browseButton_(new QPushButton(this)),
      layout_(new QHBoxLayout()) {
    setObjectName("PathInputWidget");

    layout_->setContentsMargins(0, 0, 0, 0);
    layout_->setSpacing(4);

    auto* edit = lineEdit();
    mainLayout()->removeWidget(edit);

    browseButton_->setObjectName("PathInputWidgetBrowseButton");

    constexpr auto size = QSize(26, 26);
    browseButton_->setFixedSize(size);

    browseButton_->setIcon(QIcon(":/Assets/icons/folder/openProjectLight.svg"));
    constexpr auto size1 = QSize(16, 16);
    browseButton_->setIconSize(size1);

    browseButton_->setToolTip("Open folder");

    layout_->addWidget(edit);
    layout_->addWidget(browseButton_);

    mainLayout()->insertLayout(1, layout_);

    connect(browseButton_, &QPushButton::clicked,
            this, &PathInputWidget::openFolderSlot);
}


void UI::PathInputWidget::setCheckBoxQuestion(const QString& text) {
    if (!checkBox_) {
        checkBox_ = new QCheckBox(this);
        checkBox_->setObjectName("PathInputWidgetCheckBox");

        mainLayout()->insertWidget(2, checkBox_);
    }

    checkBox_->setText(text);
    checkBox_->show();

    adjustSize();
}


bool UI::PathInputWidget::isCheckBoxChecked() const {
    return checkBox_ && checkBox_->isChecked();
}


void UI::PathInputWidget::openFolderSlot()  {
    hide();
    const auto folderPath = QFileDialog::getExistingDirectory(
        parentWidget_,
        tr("Select Folder"),
        lineEdit()->text(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    show();
    if (!folderPath.isEmpty()) {
        lineEdit()->setText(folderPath);
    }
}
