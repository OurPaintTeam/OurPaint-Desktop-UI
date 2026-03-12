#ifndef OURPAINT_DESKTOP_UI_SETTINGSWIDGET_H
#define OURPAINT_DESKTOP_UI_SETTINGSWIDGET_H

#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

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
