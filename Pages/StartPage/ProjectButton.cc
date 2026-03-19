#include "ProjectButton.h"
#include <QDebug>
#include <QLineEdit>
#include <QInputDialog>
#include <QMessageBox>


UI::ProjectButton::ProjectButton(const QString& name,
                                 const QString& path,
                                 QWidget* parent)
    : QPushButton(parent)
{
    setObjectName("ProjectButton");

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    constexpr auto h = 60;
    setMinimumHeight(h);

    layout_ = new QVBoxLayout(this);
    constexpr auto y = 10;
    constexpr auto x = 6;
    layout_->setContentsMargins(y,x,y,x);
    layout_->setSpacing(2);

    // --- TOP RIGHT BUTTONS ---
    topLayout_ = new QHBoxLayout();
    topLayout_->addStretch(); // толкает кнопки вправо

    renameBtn_ = new QPushButton("✏", this);
    renameBtn_->setObjectName("RenameBtn");
    renameBtn_->setFixedSize(20, 20);

    deleteBtn_ = new QPushButton("🗑", this);
    deleteBtn_->setObjectName("DeleteBtn");
    deleteBtn_->setFixedSize(20, 20);

    topLayout_->addWidget(renameBtn_);
    topLayout_->addWidget(deleteBtn_);

    // --- LABELS ---
    nameLabel_ = new QLabel(name,this);
    nameLabel_->setObjectName("ProjectName");

    pathLabel_ = new QLabel(path,this);
    pathLabel_->setObjectName("ProjectPath");

    // --- LAYOUT ---
    layout_->addLayout(topLayout_);
    layout_->addWidget(nameLabel_);
    layout_->addWidget(pathLabel_);

    connect(renameBtn_, &QPushButton::clicked, this, [this]() {
        qDebug() << "[ProjectButton] Rename clicked:" << getName();

        bool ok = false;
        QString newName = QInputDialog::getText(this,
                                                "Rename project",
                                                "New name:",
                                                QLineEdit::Normal,
                                                getName(),
                                                &ok);

        if (!ok || newName.isEmpty() || newName == getName()) {
            qDebug() << "[ProjectButton] Rename cancelled";
            return;
        }

        emit renameProjectTriggered(newName, getPath());
    });

    connect(deleteBtn_, &QPushButton::clicked, this, [this]() {
        qDebug() << "[ProjectButton] Delete clicked:" << getPath();

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