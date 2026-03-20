#ifndef OURPAINT_DESKTOP_UI_UIMANAGER_H
#define OURPAINT_DESKTOP_UI_UIMANAGER_H

#include "FileSystem.h"
#include "MainWindow.h"

namespace UI {
    class UIManager final : public QObject {
        Q_OBJECT

    public:
        UIManager();
        ~UIManager() override = default;

    private:
        QVector<UI::MainWindow*> mainWindows_;
        UI::FileSystem fs_;

        bool checkedOpened(const QString& projectPath) const;
        void sentCommandFromConsole(const UI::MainWindow* window, const QString& command);
        void closeApplication(const UI::MainWindow* window, const QString& projectPath);
        void openNewWindowOpenProjectSlot(const UI::MainWindow* window, const QString& projectPath);
        void openNewWindowCreateProjectSlot(const UI::MainWindow* window, const QString& projectPath);
        void openProjectThisWindowSlot(UI::MainWindow* window, const QString& projectPath) const;
        void createProjectThisWindowSlot(UI::MainWindow* window, const QString& projectPath) const;
        void openFileSlot(const UI::MainWindow* window, const QString& filePath) const;
        void renameTabSlot(const UI::MainWindow* window, const QString& oldName, const QString& newName) const;
        void removeTabSlot(const UI::MainWindow* window, const QString& tabName) const;
        void createFileSlot(const UI::MainWindow* window, const QString& fileName) const;
        void goToStartWindowSlot(UI::MainWindow* window) const;
        void renameProjectSlot(UI::MainWindow* window, const QString& newName, const QString& path) const;
        void deleteProjectSlot(UI::MainWindow* window, const QString& path) const;
        void deleteSlot(UI::MainWindow* window);

        // Tools
        void LineSlot(const UI::MainWindow* window);
        void PointSlot(const UI::MainWindow* window);
        void PolylineSlot(const UI::MainWindow* window);
        void InfiniteLineSlot(const UI::MainWindow* window);
        void CircleByDiameterSlot(const UI::MainWindow* window);
        void CircleTwoPointsSlot(const UI::MainWindow* window);
        void EllipseThreePointsSlot(const UI::MainWindow* window);
        void ArcByRadiusSlot(const UI::MainWindow* window);
        void ArcByDiameterSlot(const UI::MainWindow* window);
        void ArcByThreePointsSlot(const UI::MainWindow* window);
        void LineSettingsSlot(const UI::MainWindow* window);
        void CircleSettingsSlot(const UI::MainWindow* window);
        void ArcSettingsSlot(const UI::MainWindow* window);
        void CursorSlot(const UI::MainWindow* window);
        void SizeSlot(const UI::MainWindow* window);
        void PointLineDistanceSlot(const UI::MainWindow* window);
        void PointOnLineSlot(const UI::MainWindow* window);
        void PointPointDistanceSlot(const UI::MainWindow* window);
        void CoincidentPointsSlot(const UI::MainWindow* window);
        void LineCircleDistanceSlot(const UI::MainWindow* window);
        void LineOnCircleSlot(const UI::MainWindow* window);
        void LineInCircleSlot(const UI::MainWindow* window);
        void ParallelLinesSlot(const UI::MainWindow* window);
        void PerpendicularLinesSlot(const UI::MainWindow* window);
        void AngleBetweenLinesSlot(const UI::MainWindow* window);

        UI::MainWindow* createWindow(UI::MainWindow* parent, const UI::MainWindow::ProjectData& projectData);
        void initSignals(UI::MainWindow& window);
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_UIMANAGER_H
