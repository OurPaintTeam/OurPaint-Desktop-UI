#ifndef OURPAINT_DESKTOP_UI_TABWIDGET_H
#define OURPAINT_DESKTOP_UI_TABWIDGET_H

#include <QWidget>

class QLineEdit;
class QHBoxLayout;
class QPushButton;
class QToolButton;

namespace UI {
    class RenameTabLineEdit;


    class TabWidget : public QWidget {
        Q_OBJECT

    public:
        bool eventFilter(QObject* obj, QEvent* event) override;
        explicit TabWidget(QWidget* parent = nullptr, const qint64 fileId = -1, const QString& name="");
        void setActive(bool active);
        void startRename();
        void setName(const QString& name);
        qint64 getFileId() const;

    signals:
        void clickedTriggered(UI::TabWidget* tab);
        void draggedOutTriggered(UI::TabWidget* tab);
        void closeRequestedTriggered(UI::TabWidget* tab);
        void rightClickedTriggered(UI::TabWidget* tab, const QPoint& localPos);
        void renameTriggered(const qint64 id, const QString& newName);
        void cancelRenameTriggered();

    protected:
        void mousePressEvent(QMouseEvent* e) override;
        void mouseMoveEvent(QMouseEvent* e) override;

    private:
        QPushButton* nameButton_{nullptr};
        QToolButton* closeButton_{nullptr};
        QHBoxLayout* layout_{nullptr};
        QString name_;
        QPoint dragStartPos_;
        qint64 fileId_{-1};

       RenameTabLineEdit* edit_{nullptr};

        void finishRename();
        void cancelRename();
        QString elideText(const QString& text, int maxWidth) const;
        void changeEvent(QEvent *e);
        void translate() const;


        void applyInactiveStyle();
        void applyActiveStyle();
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_TABWIDGET_H
