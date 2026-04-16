#ifndef OURPAINT_DESKTOP_UI_TABWIDGET_H
#define OURPAINT_DESKTOP_UI_TABWIDGET_H

#include <QWidget>

class QHBoxLayout;
class QPushButton;
class QToolButton;

namespace UI {
    class TabWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit TabWidget(const QString& name, QWidget* parent = nullptr);
        [[nodiscard]] QString getName() const;
        void setName(const QString& name);
        void setActive(bool active);

    signals:
        void clickedTriggered(UI::TabWidget* tab);
        void draggedOutTriggered(UI::TabWidget* tab);
        void closeRequestedTriggered(UI::TabWidget* tab);

    protected:
        void mousePressEvent(QMouseEvent* e) override;
        void mouseMoveEvent(QMouseEvent* e) override;

    private:
        QPushButton* nameButton_{nullptr};
        QToolButton* closeButton_{nullptr};
        QHBoxLayout* layout_{nullptr};
        QString name_;
        QPoint dragStartPos_;

        void applyInactiveStyle();
        void applyActiveStyle();
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_TABWIDGET_H
