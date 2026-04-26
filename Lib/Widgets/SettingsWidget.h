#ifndef OURPAINT_DESKTOP_UI_SETTINGSWIDGET_H
#define OURPAINT_DESKTOP_UI_SETTINGSWIDGET_H
#include <QWidget>

class QLineEdit;
class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QSlider;

namespace UI {
    class SettingsWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit SettingsWidget(QWidget *parent = nullptr);

    signals:
        void settingsApplied(double value1, double value2, double value3);
    protected:
        void changeEvent(QEvent *e) override;
    private slots:
        void onApplyClicked();



    private:
        void translate() const;
        QPushButton *closeButton_{nullptr};
        QVBoxLayout *layout_{nullptr};

        QLabel *label1_{nullptr};
        QLineEdit *valueEdit1_{nullptr};
        QSlider *slider1_{nullptr};

        QLabel *label2_{nullptr};
        QLineEdit *valueEdit2_{nullptr};
        QSlider *slider2_{nullptr};

        QLabel *label3_{nullptr};
        QLineEdit *valueEdit3_{nullptr};
        QSlider *slider3_{nullptr};

        QPushButton *applyButton_{nullptr};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_SETTINGSWIDGET_H
