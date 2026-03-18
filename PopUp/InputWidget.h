#ifndef OURPAINT_DESKTOP_UI_INPUTWIDGET_H
#define OURPAINT_DESKTOP_UI_INPUTWIDGET_H

#include <QWidget>

class QHBoxLayout;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPoint;
class QPushButton;
class QVBoxLayout;

namespace UI {
    class InputWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit InputWidget(const QString& promptText, QWidget* parent = nullptr);

        void setCheckBoxQuestion(const QString& question);
        bool isCheckBoxChecked() const;

    signals:
        void inputEnteredTriggered(const QString& text);

    private slots:
        void onOkClickedSlot();

    protected:
        void mousePressEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
        void paintEvent(QPaintEvent* event) override;

    private:
        QLabel* label_{nullptr};
        QLineEdit* lineEdit_{nullptr};
        QPushButton* okButton_{nullptr};
        QPushButton* closeButton_{nullptr};
        QCheckBox* checkBox_{nullptr};
        QHBoxLayout* buttonLayout_{nullptr};

        QVBoxLayout* mainLayout_{nullptr};
        QHBoxLayout* headerLayout_{nullptr};

        bool dragging_{false};
        QPoint dragPosition_{};

        const QColor backgroundColor_ = QColor(QStringLiteral("#3f3e46"));
        const QColor borderColor_ = QColor(QStringLiteral("#3e3d42"));
    };
} // namespace UI


#endif //OURPAINT_DESKTOP_UI_INPUTWIDGET_H
