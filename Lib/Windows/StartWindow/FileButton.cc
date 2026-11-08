#include "FileButton.h"

#include <QEvent>
#include <QInputDialog>
#include <QMessageBox>

#include "ConfirmDialog.h"
#include "RenameTabLineEdit.h"
#include "Utils.h"


UI::FileButton::FileButton( const ProjectData& file,
                            QWidget* parent)
    : QPushButton(parent),
      nameLabel_(new QLabel(file.name, this)),
      pathLabel_(new QLabel(file.path, this)),
      layout_(new QVBoxLayout(this)) {
    setObjectName("ProjectButton");

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    constexpr auto h = 60;
    setMinimumHeight(h);

    constexpr auto y = 10;
    constexpr auto x = 6;
    layout_->setContentsMargins(y, x, y, x);
    layout_->setSpacing(2);

    constexpr auto size = QSize(20, 20);
    renameBtn_ = new QPushButton("✏", this);
    renameBtn_->setObjectName("RenameBtnProject");
    renameBtn_->setFixedSize(size);

    deleteBtn_ = new QPushButton("🗑", this);
    deleteBtn_->setObjectName("DeleteBtnProject");
    deleteBtn_->setFixedSize(size);

    nameLabel_->setObjectName("ProjectName");
    pathLabel_->setObjectName("ProjectPath");

    nameContainer_ = new QWidget(this);
    nameContainer_->setObjectName("NameContainerProject");
    nameStack_ = new QStackedLayout(nameContainer_);
    nameStack_->setContentsMargins(0, 0, 0, 0);

    nameStack_->addWidget(nameLabel_);

    edit_ = new RenameTabLineEdit(this);
    edit_->setObjectName("ProjectNameEdit");
    edit_->setFrame(false);

    edit_->setMaximumWidth(200);
    nameLabel_->setMaximumWidth(200);

    nameContainer_->setMinimumHeight(20);
    edit_->setMinimumHeight(20);
    nameLabel_->setMinimumHeight(20);

    nameStack_->addWidget(edit_);

    QHBoxLayout* topSectionLayout = new QHBoxLayout();
    topSectionLayout->setContentsMargins(0, 0, 0, 0);
    topSectionLayout->setSpacing(5);

    topSectionLayout->addWidget(nameContainer_, 1);

    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(2);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(renameBtn_);
    buttonsLayout->addWidget(deleteBtn_);

    topSectionLayout->addLayout(buttonsLayout, 0);

    layout_->addLayout(topSectionLayout);
    layout_->addWidget(pathLabel_);

    nameLabel_->installEventFilter(this);
    nameLabel_->setToolTip(file.name);
    pathLabel_->setToolTip(file.path);

    edit_->setContentsMargins(0, 0, 0, 0);
    nameLabel_->setContentsMargins(0, 0, 0, 0);

    edit_->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    nameLabel_->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    nameStack_->setCurrentWidget(nameLabel_);

    fileId_ = file.id;

    connect(edit_, &RenameTabLineEdit::focusLost, this, &FileButton::cancelRenameInline);
    connect(edit_, &QLineEdit::returnPressed, this, &FileButton::finishRenameInline);
    connect(renameBtn_, &QPushButton::clicked, this, &FileButton::startRenameInline);

    connect(deleteBtn_, &QPushButton::clicked, this, [this]() {
        auto* confirmDialog = new UI::ConfirmDialog("Delete file " + getName() + "?", "Yes", "No", this);
        connect(confirmDialog, &UI::ConfirmDialog::yesClicked, this, [this]() {
            emit deleteFileTriggered(fileId_);
        });

        QPoint cursorPos = QCursor::pos();
        confirmDialog->move(cursorPos - QPoint(confirmDialog->width() / 2, 10));

        confirmDialog->show();
    });

    connect(this, &QPushButton::clicked, this, [this]() {
        emit pressFileTriggered(fileId_);
    });

    translate();
}

bool UI::FileButton::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == nameLabel_) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            startRenameInline();
            return true;
        }
    }

    return QPushButton::eventFilter(obj, event);
}

void UI::FileButton::startRenameInline()
{
    editing_ = true;
    edit_->setText(getName());
    nameStack_->setCurrentWidget(edit_);
    edit_->setFocus();
    edit_->selectAll();
}

void UI::FileButton::finishRenameInline()
{
    if (!editing_) {
        return;
    }

    editing_ = false;

    const QString newName = edit_->text().trimmed();

    if (!newName.isEmpty() && newName != getName()) {
        FileData file = {fileId_, newName};
        emit renameFileTriggered(file);
    }

    nameStack_->setCurrentWidget(nameLabel_);
}


void UI::FileButton::cancelRenameInline()
{
    editing_ = false;
    nameStack_->setCurrentWidget(nameLabel_);
}

QString UI::FileButton::getName() const {
    return nameLabel_->text();
}

QString UI::FileButton::getPath() const {
    return pathLabel_->text();
}

void UI::FileButton::changeEvent(QEvent* e) {
    if (e && e->type() == QEvent::LanguageChange) {
        translate();
    }
    QWidget::changeEvent(e);
}

void UI::FileButton::translate() const {
    if (renameBtn_) {
        renameBtn_->setToolTip(UI::FileButton::tr("Rename file"));
    }
    if (deleteBtn_) {
        deleteBtn_->setToolTip(UI::FileButton::tr("Delete file"));
    }
}