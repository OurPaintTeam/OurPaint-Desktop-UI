#include "ProjectButton.h"

#include <QEvent>
#include <QInputDialog>
#include <QMessageBox>

#include "ConfirmDialog.h"
#include "RenameTabLineEdit.h"

UI::ProjectButton::ProjectButton(const QString& name,
                                 const QString& path,
                                 QWidget* parent)
    : QPushButton(parent),
      nameLabel_(new QLabel(name, this)),
      pathLabel_(new QLabel(path, this)),
      layout_(new QVBoxLayout(this)),
      topLayout_(new QHBoxLayout()) {
    setObjectName("ProjectButton");

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    constexpr auto h = 60;
    setMinimumHeight(h);

    constexpr auto y = 10;
    constexpr auto x = 6;
    layout_->setContentsMargins(y, x, y, x);
    layout_->setSpacing(2);

    topLayout_->addStretch();
    constexpr auto size = QSize(20, 20);
    renameBtn_ = new QPushButton("✏", this);
    renameBtn_->setObjectName("RenameBtnProject");
    renameBtn_->setFixedSize(size);

    deleteBtn_ = new QPushButton("🗑", this);
    deleteBtn_->setObjectName("DeleteBtnProject");
    deleteBtn_->setFixedSize(size);

    topLayout_->addWidget(renameBtn_);
    topLayout_->addWidget(deleteBtn_);

    nameLabel_->setObjectName("ProjectName");
    pathLabel_->setObjectName("ProjectPath");

    nameContainer_ = new QWidget(this);
    nameContainer_->setObjectName("NameContainerProject");
    nameContainer_->setAttribute(Qt::WA_TranslucentBackground);
    nameContainer_->setAutoFillBackground(false);
    nameStack_ = new QStackedLayout(nameContainer_);
    nameStack_->setContentsMargins(0, 0, 0, 0);

    nameStack_->addWidget(nameLabel_);

    edit_ = new RenameTabLineEdit(this);
    edit_->setObjectName("ProjectNameEdit");
    edit_->hide();

    edit_->setMaximumWidth(200);
    nameLabel_->setMaximumWidth(200);

    edit_->setMinimumHeight(15);
    nameLabel_->setMinimumHeight(15);

    nameStack_->addWidget(edit_);
    layout_->addLayout(topLayout_);
    layout_->addWidget(nameContainer_);
    layout_->addWidget(pathLabel_);

    nameLabel_->installEventFilter(this);

    connect(edit_, &RenameTabLineEdit::focusLost, this, &ProjectButton::cancelRenameInline);
    connect(edit_, &QLineEdit::returnPressed, this, &ProjectButton::finishRenameInline);
    connect(renameBtn_, &QPushButton::clicked, this, &ProjectButton::startRenameInline);



    connect(deleteBtn_, &QPushButton::clicked, this, [this]() {
        auto* confirmDialog = new UI::ConfirmDialog("Delete project " + getName() + "?", "Yes", "No", this);
        connect(confirmDialog, &UI::ConfirmDialog::yesClicked, this, [this]() {
            emit deleteProjectTriggered(getPath());
        });

        QPoint cursorPos = QCursor::pos();
        confirmDialog->move(cursorPos - QPoint(confirmDialog->width() / 2, 10));

        confirmDialog->show();
    });

    connect(this, &QPushButton::clicked, this, [this]() {
        emit pressProjectTriggered(getPath());
    });
}

bool UI::ProjectButton::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == nameLabel_) {
        if (event->type() == QEvent::MouseButtonDblClick) {
            startRenameInline();
            return true;
        }
    }

    return QPushButton::eventFilter(obj, event);
}

void UI::ProjectButton::startRenameInline()
{
    editing_ = true;
    edit_->setText(getName());
    nameStack_->setCurrentWidget(edit_);
    edit_->setFocus();
    edit_->selectAll();
}

void UI::ProjectButton::finishRenameInline()
{
    if (!editing_)
        return;

    editing_ = false;

    const QString newName = edit_->text().trimmed();

    if (!newName.isEmpty() && newName != getName()) {
        emit renameProjectTriggered(newName, getPath());
        nameLabel_->setText(newName);
    }

    nameStack_->setCurrentWidget(nameLabel_);
}

void UI::ProjectButton::cancelRenameInline()
{
    editing_ = false;
    nameStack_->setCurrentWidget(nameLabel_);
}

QString UI::ProjectButton::getName() const {
    return nameLabel_->text();
}

QString UI::ProjectButton::getPath() const {
    return pathLabel_->text();
}