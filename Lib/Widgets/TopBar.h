#ifndef OURPAINT_DESKTOP_UI_TOPBAR_H
#define OURPAINT_DESKTOP_UI_TOPBAR_H

#include <QWidget>

class QHBoxLayout;
class QPushButton;

namespace UI {
    class TopBar : public QWidget {
        Q_OBJECT

    public:
        explicit TopBar(QWidget* parent = nullptr);

        void addLeftWidget(QWidget* widget) const;
        void addCenterWidget(QWidget* widget) const;

    protected:
        QPushButton* createWindowButton();
        QPushButton* createWindowButton(const QString& text);
        QPushButton* createWindowButton(const QIcon& icon);
        void mousePressEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;

    private:
        QHBoxLayout* mainLayout_{nullptr};
        QWidget* leftContainer_{nullptr};
        QHBoxLayout* rightCombinedLayout_{nullptr};
        QWidget* centerContainer_{nullptr};
        QWidget* rightContainer_{nullptr};
        QWidget* combinedWidget_{nullptr};

        QHBoxLayout* leftLayout_{nullptr};
        QHBoxLayout* centerLayout_{nullptr};
        QHBoxLayout* rightLayout_{nullptr};

        QPushButton* minButton_{nullptr};
        QPushButton* maxButton_{nullptr};
        QPushButton* closeButton_{nullptr};

        void setupLayouts();
        void setupWindowButtons();
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_TOPBAR_H
