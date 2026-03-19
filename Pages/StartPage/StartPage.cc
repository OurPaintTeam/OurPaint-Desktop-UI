#include "StartPage.h"

#include "AnimationArea.h"
#include "InputWidget.h"
#include "ProjectsListWidget.h"
#include "TopBarStart.h"

#include <QFileDialog>
#include <QVBoxLayout>


UI::StartPage::StartPage(QWidget* parent)
    : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
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


void UI::StartPage::onOpenProjectSlot() {
    const auto folderPath = QFileDialog::getExistingDirectory(
        this,
        tr("Open File or Project"),
        QString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (folderPath.isEmpty()) {
        return;
    }

    emit openProjectInThisWindowTriggered(folderPath);
}


void UI::StartPage::onCreateProjectSlot() {
    auto* prompt = new InputWidget("Project Path:", this);

    const auto rect = this->rect();
    const auto size = prompt->sizeHint();

    prompt->move(rect.center().x() - size.width() / 2,
                 rect.center().y() - size.height() / 2);

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
    mainLayout_ = new QVBoxLayout(this);
    mainArea_ = new QWidget(this);
    contentLayout_ = new QHBoxLayout(mainArea_);

    topBar_ = new TopBarStart(this);

    projectsListWidget_ = new ProjectsListWidget(mainArea_);
    animationArea_ = new AnimationArea(mainArea_);

    mainLayout_->setContentsMargins(0, 0, 0, 0);
    mainLayout_->setSpacing(0);

    contentLayout_->setContentsMargins(0, 0, 0, 0);
    contentLayout_->setSpacing(0);

    mainLayout_->addWidget(topBar_);
    mainLayout_->addWidget(mainArea_);

    contentLayout_->addWidget(projectsListWidget_);
    contentLayout_->addWidget(animationArea_, 1);
}
