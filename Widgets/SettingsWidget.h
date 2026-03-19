#ifndef OURPAINT_DESKTOP_UI_SETTINGSWIDGET_H
#define OURPAINT_DESKTOP_UI_SETTINGSWIDGET_H

#include <QWidget>

class QPushButton;
class QVBoxLayout;

namespace UI {
    class SettingsWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit SettingsWidget(QWidget* parent = nullptr);

    private:
        QPushButton* closeButton_{nullptr};
        QVBoxLayout* layout_{nullptr};
    };
} // namespace UI

#endif // OURPAINT_DESKTOP_UI_SETTINGSWIDGET_H
