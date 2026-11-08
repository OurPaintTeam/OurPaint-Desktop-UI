#include "FilesListWidget.h"

#include <QEvent>
#include <QLineEdit>
#include <QScrollArea>

#include "FileButton.h"
#include "Utils.h"


UI::FilesListWidget::FilesListWidget(QWidget* parent)
    : QWidget(parent),
      rootLayoutPtr_(new QVBoxLayout(this)),
      openPanelPtr_(new QWidget(this)),
      panelLayoutPtr_(new QVBoxLayout(openPanelPtr_)), inputContainerPtr_(new QWidget(openPanelPtr_)),
      inputLayoutPtr_(new QHBoxLayout(inputContainerPtr_)),
      projectInput_(new QLineEdit(inputContainerPtr_)),
      createButtonPtr_(new QPushButton(inputContainerPtr_)), openButtonPtr_(new QPushButton(inputContainerPtr_)),
      linePtr_(new QFrame(openPanelPtr_)),
      scrollAreaPtr_(new QScrollArea(openPanelPtr_)),
      scrollContent_(new QWidget()),
      filesLayout_(new QVBoxLayout(scrollContent_)) {
    setMinimumWidth(WIDGET_WIDTH);
    setMaximumWidth(WIDGET_WIDTH);
    setObjectName("projectsListWidget");

    rootLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    rootLayoutPtr_->setSpacing(0);
    rootLayoutPtr_->setObjectName("rootLayout");

    openPanelPtr_->setObjectName("openPanel");

    panelLayoutPtr_->setContentsMargins(PANEL_MARGIN, PANEL_MARGIN, PANEL_MARGIN, PANEL_MARGIN);
    panelLayoutPtr_->setSpacing(PANEL_MARGIN);
    panelLayoutPtr_->setObjectName("panelLayout");

    rootLayoutPtr_->addWidget(openPanelPtr_);

    inputContainerPtr_->setObjectName("inputContainer");

    inputLayoutPtr_->setContentsMargins(0, 0, 0, 0);
    constexpr auto x = 5;
    inputLayoutPtr_->setSpacing(x);
    inputLayoutPtr_->setObjectName("inputLayout");

    projectInput_->setMaximumHeight(INPUT_MAX_HEIGHT);
    projectInput_->setObjectName("projectInput");

    inputLayoutPtr_->addWidget(projectInput_, 1);

    createButtonPtr_->setIcon(QIcon(":/Assets/icons/folder/createFile.svg"));
    createButtonPtr_->setObjectName("createButtonStartWindow");
    createButtonPtr_->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    connect(createButtonPtr_, &QPushButton::clicked, this, &FilesListWidget::pressCreateButtonTriggered);

    openButtonPtr_->setIcon(QIcon(":/Assets/icons/folder/openFileDark.svg"));
    openButtonPtr_->setObjectName("createButtonStartWindow");
    openButtonPtr_->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    connect(openButtonPtr_, &QPushButton::clicked, this, &FilesListWidget::pressOpenButtonTriggered);

    inputLayoutPtr_->addWidget(openButtonPtr_);
    inputLayoutPtr_->addWidget(createButtonPtr_);

    panelLayoutPtr_->addWidget(inputContainerPtr_);

    linePtr_->setFrameShape(QFrame::HLine);
    linePtr_->setFrameShadow(QFrame::Plain);
    linePtr_->setObjectName("projectsLine");

    panelLayoutPtr_->addWidget(linePtr_);

    scrollAreaPtr_->setWidgetResizable(true);
    scrollAreaPtr_->setObjectName("projectsScrollArea");

    scrollContent_->setObjectName("projectsScrollContent");

    filesLayout_->setSpacing(0);
    filesLayout_->setContentsMargins(
        CONTENT_MARGINS_LEFT,
        CONTENT_MARGINS_TOP,
        CONTENT_MARGINS_RIGHT,
        CONTENT_MARGINS_BOTTOM
    );
    filesLayout_->setSpacing(LAYOUT_SPACING);
    filesLayout_->addStretch();
    filesLayout_->setObjectName("filesLayout_");

    scrollAreaPtr_->setWidget(scrollContent_);

    panelLayoutPtr_->addWidget(scrollAreaPtr_, 1);

    connect(projectInput_, &QLineEdit::textChanged,
            this, &FilesListWidget::filterProjectsSlot);

    translate();
}


void UI::FilesListWidget::setFiles(const QVector<ProjectData>& files) {
    for (auto* project: allFiles_) {
        filesLayout_->removeWidget(project);
        project->deleteLater();
    }
    allFiles_.clear();

    for (const auto& file : files) {
        auto* project = new FileButton(file, scrollContent_);

        connect(project, &FileButton::pressFileTriggered, this, &FilesListWidget::pressFileTriggered);
        connect(project, &FileButton::deleteFileTriggered, this, &FilesListWidget::deleteFileTriggered);
        connect(project, &FileButton::renameFileTriggered, this, &FilesListWidget::renameFileTriggered);

        allFiles_.append(project);
        filesLayout_->insertWidget(filesLayout_->count() - 1, project);
    }
}


void UI::FilesListWidget::filterProjectsSlot(const QString& text) {
    for (auto* const project: allFiles_) {
        filesLayout_->removeWidget(project);
        project->hide();
    }

    if (text.isEmpty()) {
        for (auto* const project: allFiles_) {
            filesLayout_->addWidget(project);
            project->show();
        }
        return;
    }

    auto sortedProjects = allFiles_;
    auto lowerText = text.toLower();

    std::sort(sortedProjects.begin(), sortedProjects.end(),
              [lowerText](const FileButton* a, const FileButton* b) {
                  const auto nameA = a->getName().toLower();
                  const auto nameB = b->getName().toLower();

                  const bool aStartsWith = nameA.startsWith(lowerText);
                  const bool bStartsWith = nameB.startsWith(lowerText);

                  if (aStartsWith && !bStartsWith) { return true; }
                  if (!aStartsWith && bStartsWith) { return false; }

                  const qsizetype posA = nameA.indexOf(lowerText);

                  if (const qsizetype posB = nameB.indexOf(lowerText); posA != -1 && posB != -1) {
                      if (posA != posB) {
                          return posA < posB;
                      }
                  } else if (posA != -1) {
                      return true;
                  } else if (posB != -1) {
                      return false;
                  }

                  return nameA < nameB;
              });

    for (auto* const project: sortedProjects) {
        if (project->getName().toLower().contains(lowerText)) {
            filesLayout_->addWidget(project);
            project->show();
        }
    }

    for (auto* const project: allFiles_) {
        if (!project->getName().toLower().contains(lowerText)) {
            filesLayout_->addWidget(project);
            project->show();
        }
    }
}


void UI::FilesListWidget::changeEvent(QEvent* e) {
    if (e && e->type() == QEvent::LanguageChange) {
        translate();
    }
    QWidget::changeEvent(e);
}

void UI::FilesListWidget::translate() const {
    if (createButtonPtr_) {
        createButtonPtr_->setToolTip(UI::FilesListWidget::tr("Create new file"));
    }
    if (openButtonPtr_) {
        openButtonPtr_->setToolTip(UI::FilesListWidget::tr("Open file"));
    }
    if (projectInput_) {
        projectInput_->setPlaceholderText(UI::FilesListWidget::tr("Enter the file name..."));
    }
}