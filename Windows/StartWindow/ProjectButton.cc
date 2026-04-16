#include "ProjectButton.h"

#include <QInputDialog>
#include <QMessageBox>


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
    renameBtn_->setObjectName("RenameBtn");
    renameBtn_->setFixedSize(size);

    deleteBtn_ = new QPushButton("🗑", this);
    deleteBtn_->setObjectName("DeleteBtn");
    deleteBtn_->setFixedSize(size);

    topLayout_->addWidget(renameBtn_);
    topLayout_->addWidget(deleteBtn_);

    nameLabel_->setObjectName("ProjectName");

    pathLabel_->setObjectName("ProjectPath");

    layout_->addLayout(topLayout_);
    layout_->addWidget(nameLabel_);
    layout_->addWidget(pathLabel_);

    connect(renameBtn_, &QPushButton::clicked, this, [this]() {
        bool ok = false;
        const auto newName = QInputDialog::getText(this,
                                                   "Rename project",
                                                   "New name:",
                                                   QLineEdit::Normal,
                                                   getName(),
                                                   &ok);

        if (!ok || newName.isEmpty() || newName == getName()) {
            return;
        }

        emit renameProjectTriggered(newName, getPath());
    });

    connect(deleteBtn_, &QPushButton::clicked, this, [this]() {
        if (QMessageBox::question(this,
                                  "Delete project",
                                  "Delete project?\n" + getName(),
                                  QMessageBox::Yes | QMessageBox::No)
            != QMessageBox::Yes) {
            return;
        }

        emit deleteProjectTriggered(getPath());
    });

    connect(this, &QPushButton::clicked, this, [this]() {
        emit pressProjectTriggered(getPath());
    });
}


QString UI::ProjectButton::getName() const {
    return nameLabel_->text();
}


QString UI::ProjectButton::getPath() const {
    return pathLabel_->text();
}
