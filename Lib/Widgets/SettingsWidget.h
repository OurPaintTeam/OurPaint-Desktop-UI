#ifndef OURPAINT_DESKTOP_UI_SETTINGSWIDGET_H
#define OURPAINT_DESKTOP_UI_SETTINGSWIDGET_H

#include <QFrame>

class QCheckBox;
class QCloseEvent;
class QComboBox;
class QLineEdit;
class QLabel;
class QPushButton;
class QSlider;
class QVBoxLayout;

namespace UI {

    class SettingsSystem;

    class SettingsWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit SettingsWidget(SettingsSystem* settings,
                                QWidget* parent = nullptr);

        signals:
            void settingsChanged();

    protected:
        void changeEvent(QEvent* e) override;
        void closeEvent(QCloseEvent* e) override;

    private:
        void loadFromSettings();
        void wireSignals();
        void translate() const;
        void commitGridStep(double value);
        void commitSnapStep(double value);

        SettingsSystem* settings_{nullptr};

        QPushButton* closeButton_{nullptr};
        QVBoxLayout* layout_{nullptr};

        QCheckBox*   gridCheck_{nullptr};
        QLabel*      gridStepLabel_{nullptr};
        QLineEdit*   gridStepEdit_{nullptr};
        QSlider*     gridStepSlider_{nullptr};
        QLabel* titleLabel_{nullptr};

        QCheckBox*   snapCheck_{nullptr};
        QLabel*      snapStepLabel_{nullptr};
        QLineEdit*   snapStepEdit_{nullptr};
        QSlider*     snapStepSlider_{nullptr};

        QLabel*      languageLabel_{nullptr};
        QComboBox*   languageCombo_{nullptr};

        QCheckBox* notificationsCheck_{nullptr};
        // applyButton_ удалён
    };

} // namespace UI

#endif