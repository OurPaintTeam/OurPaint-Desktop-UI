#include "StartPage.h"

#include "AnimationArea.h"
#include "InputWidget.h"
#include "ProjectsListWidget.h"
#include "TopBarStart.h"

#include <QFileDialog>
#include <QHBoxLayout>

#include "PathInputWidget.h"


UI::StartPage::StartPage(QWidget* parent)
    : BasePage(parent) {
    setObjectName("StartPage");
    initUI();
    connect(projectsListWidget_, &ProjectsListWidget::pressProjectTriggered, this,
            &StartPage::openProjectInThisWindowTriggered);
    connect(projectsListWidget_, &ProjectsListWidget::renameProjectTriggered, this, &StartPage::renameProjectTriggered);
    connect(projectsListWidget_, &ProjectsListWidget::deleteProjectTriggered, this, &StartPage::deleteProjectTriggered);

    connect(projectsListWidget_, &ProjectsListWidget::pressOpenButtonTriggered, this, &StartPage::onOpenProjectSlot);
    connect(projectsListWidget_, &ProjectsListWidget::pressCreateButtonTriggered, this,
            &StartPage::onCreateProjectSlot);
}


void UI::StartPage::setProjectsList(const QVector<QPair<QString, QString> >& projects) const {
    projectsListWidget_->setProjects(projects);
}


void UI::StartPage::setDefaultProjectsPath(const QString& projectPath){
    defaultProjectsPath_ = projectPath;
}


void UI::StartPage::onOpenProjectSlot() {
    const auto folderPath = QFileDialog::getExistingDirectory(
        this,
        tr("Open File or Project"),
        defaultProjectsPath_,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (folderPath.isEmpty()) {
        return;
    }

    emit openProjectInThisWindowTriggered(folderPath);
}


void UI::StartPage::onCreateProjectSlot() {
    auto* prompt = new PathInputWidget("Project Path:", this);
    const auto rect = this->rect();
    const auto size = prompt->sizeHint();

    prompt->move(rect.center().x() - size.width() / 2,
                 rect.center().y() - size.height() / 2);

    prompt->setDefaultText(defaultProjectsPath_);
    prompt->show();

    connect(prompt, &InputWidget::inputEnteredTriggered, this, [this,prompt](const QString& text) {
        if (text.isEmpty()) {
            return;
        }
        prompt->deleteLater();

        emit createProjectInThisWindowTriggered(text);
    });
}


void UI::StartPage::initUI() {
    initBasePage();
    contentLayout_ = new QHBoxLayout(mainArea_);

    topBar_ = new TopBarStart(this);

    projectsListWidget_ = new ProjectsListWidget(mainArea_);
    animationArea_ = new AnimationArea(mainArea_);

    contentLayout_->setContentsMargins(0, 0, 0, 0);
    contentLayout_->setSpacing(0);

    mainLayout_->insertWidget(0, topBar_);

    contentLayout_->addWidget(projectsListWidget_);
    contentLayout_->addWidget(animationArea_, 1);
}
