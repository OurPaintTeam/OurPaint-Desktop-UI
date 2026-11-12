#ifndef OURPAINT_DESKTOP_UI_TABWIDGET_H
#define OURPAINT_DESKTOP_UI_TABWIDGET_H

#include <QWidget>

#include "Utils.h"

class QLineEdit;
class QHBoxLayout;
class QPushButton;
class QToolButton;

namespace UI {
    struct FileData;
    class RenameTabLineEdit;


    class TabWidget : public QWidget {
        Q_OBJECT

    public:
        bool eventFilter(QObject* obj, QEvent* event) override;
        explicit TabWidget(QWidget* parent = nullptr, FileData  data = FileData());
        void setActive(bool active);
        void startRename();
        void setName(const QString& name);
        qint64 getFileId() const;
        void setOwnerWindow(QWidget* win);

    signals:
        void clickedTriggered(UI::TabWidget* tab);
        void draggedOutTriggered(UI::TabWidget* tab);
        void closeRequestedTriggered(UI::TabWidget* tab);
        void rightClickedTriggered(UI::TabWidget* tab, const QPoint& localPos);
        void renameTriggered(UI::FileData& fileData);
        void cancelRenameTriggered();

    protected:
        void mousePressEvent(QMouseEvent* e) override;
        void mouseMoveEvent(QMouseEvent* e) override;

    private:
        QPushButton* nameButton_{nullptr};
        QToolButton* closeButton_{nullptr};
        QHBoxLayout* layout_{nullptr};
        QPoint dragStartPos_;
        QWidget* ownerWindow_{nullptr};

        FileData fileData_;
       RenameTabLineEdit* edit_{nullptr};

        void finishRename();
        void startDrag();
        void cancelRename();
        QString elideText(const QString& text, int maxWidth) const;
        void changeEvent(QEvent *e);
        void translate() const;


        void applyInactiveStyle();
        void applyActiveStyle();
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_TABWIDGET_H
