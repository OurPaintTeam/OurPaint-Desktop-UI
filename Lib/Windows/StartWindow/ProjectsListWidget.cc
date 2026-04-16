#include "ProjectsListWidget.h"

#include <QLineEdit>
#include <QScrollArea>

#include "ProjectButton.h"
#include "StartPage.h"


UI::ProjectsListWidget::ProjectsListWidget(QWidget* parent)
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
      projectsLayout_(new QVBoxLayout(scrollContent_)) {
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

    projectInput_->setPlaceholderText("Enter the project name...");
    projectInput_->setMaximumHeight(INPUT_MAX_HEIGHT);
    projectInput_->setObjectName("projectInput");

    inputLayoutPtr_->addWidget(projectInput_, 1);

    createButtonPtr_->setIcon(QIcon(":/Assets/icons/folder/createProject.svg"));
    createButtonPtr_->setObjectName("createButtonStartWindow");
    createButtonPtr_->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    createButtonPtr_->setToolTip("Create Project");
    connect(createButtonPtr_, &QPushButton::clicked, this, &ProjectsListWidget::pressCreateButtonTriggered);

    openButtonPtr_->setIcon(QIcon(":/Assets/icons/folder/openProjectDark.svg"));
    openButtonPtr_->setObjectName("createButtonStartWindow");
    openButtonPtr_->setFixedSize(BUTTON_SIZE, BUTTON_SIZE);
    openButtonPtr_->setToolTip("Open Project");
    connect(openButtonPtr_, &QPushButton::clicked, this, &ProjectsListWidget::pressOpenButtonTriggered);

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

    projectsLayout_->setSpacing(0);
    projectsLayout_->setContentsMargins(
        CONTENT_MARGINS_LEFT,
        CONTENT_MARGINS_TOP,
        CONTENT_MARGINS_RIGHT,
        CONTENT_MARGINS_BOTTOM
    );
    projectsLayout_->setSpacing(LAYOUT_SPACING);
    projectsLayout_->addStretch();
    projectsLayout_->setObjectName("projectsLayout_");

    scrollAreaPtr_->setWidget(scrollContent_);

    panelLayoutPtr_->addWidget(scrollAreaPtr_, 1);

    connect(projectInput_, &QLineEdit::textChanged,
            this, &ProjectsListWidget::filterProjectsSlot);
}


void UI::ProjectsListWidget::setProjects(const QVector<QPair<QString, QString> >& projects) {
    for (auto* project: allProjects_) {
        projectsLayout_->removeWidget(project);
        project->deleteLater();
    }
    allProjects_.clear();

    for (const auto& [name, path]: projects) {
        auto* project = new ProjectButton(name, path, scrollContent_);

        connect(project, &ProjectButton::pressProjectTriggered, this, &ProjectsListWidget::pressProjectTriggered);
        connect(project, &ProjectButton::deleteProjectTriggered, this, &ProjectsListWidget::deleteProjectTriggered);
        connect(project, &ProjectButton::renameProjectTriggered, this, &ProjectsListWidget::renameProjectTriggered);

        allProjects_.append(project);
        projectsLayout_->insertWidget(projectsLayout_->count() - 1, project);
    }
}


void UI::ProjectsListWidget::filterProjectsSlot(const QString& text) {
    for (auto* const project: allProjects_) {
        projectsLayout_->removeWidget(project);
        project->hide();
    }

    if (text.isEmpty()) {
        for (auto* const project: allProjects_) {
            projectsLayout_->addWidget(project);
            project->show();
        }
        return;
    }

    auto sortedProjects = allProjects_;
    auto lowerText = text.toLower();

    std::sort(sortedProjects.begin(), sortedProjects.end(),
              [lowerText](const ProjectButton* a, const ProjectButton* b) {
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
            projectsLayout_->addWidget(project);
            project->show();
        }
    }

    for (auto* const project: allProjects_) {
        if (!project->getName().toLower().contains(lowerText)) {
            projectsLayout_->addWidget(project);
            project->show();
        }
    }
}
