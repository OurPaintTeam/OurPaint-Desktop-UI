#ifndef OURPAINT_DESKTOP_UI_TABWIDGET_H
#define OURPAINT_DESKTOP_UI_TABWIDGET_H

#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QWidget>


namespace UI {
    class TabWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit TabWidget(const QString& name, QWidget* parent = nullptr);
        [[nodiscard]] QString getName() const;
        [[nodiscard]] QString name() const;
        void setActive(bool active);

    signals:
        void clicked(UI::TabWidget* tab);
        void draggedOut(UI::TabWidget* tab);
        void closeRequested(UI::TabWidget* tab);

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
