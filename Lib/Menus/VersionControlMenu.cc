#include "VersionControlMenu.h"

#include <QEvent>


UI::VersionControlMenu::VersionControlMenu(QWidget *parent)
    : CustomMenu(parent) {
    setObjectName(QStringLiteral("VersionControlMenu"));
    setAttribute(Qt::WA_StyledBackground, true);

    init_ = addAction("");
    create_ = addAction("");

    addSeparator();

    push_ = addAction("");
    pull_ = addAction("");
    commit_ = addAction("");

    connect(init_,   &QAction::triggered, this, [this] {
        emit versionTriggered(UI::version::init);
    });
    connect(create_, &QAction::triggered, this, [this] {
        emit versionTriggered(UI::version::create);
    });
    connect(push_,   &QAction::triggered, this, [this] {
        emit versionTriggered(UI::version::push);
    });
    connect(pull_,   &QAction::triggered, this, [this] {
        emit versionTriggered(UI::version::pull);
    });
    connect(commit_, &QAction::triggered, this, [this] {
        emit versionTriggered(UI::version::commit);
    });

    translate();
}


void UI::VersionControlMenu::translate() const {
    if (init_) {
        init_->setText(UI::VersionControlMenu::tr("Init"));
    }
    if (create_) {
        create_->setText(UI::VersionControlMenu::tr("Create"));
    }
    if (push_) {
        push_->setText(UI::VersionControlMenu::tr("Push"));
    }
    if (pull_) {
        pull_->setText(UI::VersionControlMenu::tr("Pull"));
    }
    if (commit_) {
        commit_->setText(UI::VersionControlMenu::tr("Commit"));
    }
}
