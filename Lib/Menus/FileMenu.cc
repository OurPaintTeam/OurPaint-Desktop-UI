#include "FileMenu.h"

#include <QEvent>


UI::FileMenu::FileMenu(QWidget *parent)
    : UI::CustomMenu(parent) {
    createFile_ = addAction(QIcon(":/Assets/icons/menus/fileCreate.ico"), "");
    openFile_ = addAction(QIcon(":/Assets/icons/menus/fileCreate.ico"), "");
    exportFile_ = addAction("Export file to...");

    addSeparator();

    script_ = addAction("");

    setObjectName(QStringLiteral("ProjectMenu"));
    setAttribute(Qt::WA_StyledBackground, true);

    connect(createFile_, &QAction::triggered, this, &FileMenu::createFileTriggered);
    connect(openFile_, &QAction::triggered, this, &FileMenu::openFileTriggered);
    connect(exportFile_, &QAction::triggered, this, &FileMenu::exportFileTriggered);
    connect(script_, &QAction::triggered, this, &FileMenu::scriptTriggered);

    translate();
}


void UI::FileMenu::translate() const {
    if (createFile_) {
        createFile_->setText(UI::FileMenu::tr("Create new file"));
    }
    if (openFile_) {
        openFile_->setText(UI::FileMenu::tr("Open file"));
    }
    if (exportFile_) {
        exportFile_->setText(UI::FileMenu::tr("Export file to..."));
    }
    if (script_) {
        script_->setText(UI::FileMenu::tr("Script"));
    }
}
