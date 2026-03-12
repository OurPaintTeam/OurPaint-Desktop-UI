#ifndef OURPAINT_DESKTOP_UI_INFORMATIONPANEL_H
#define OURPAINT_DESKTOP_UI_INFORMATIONPANEL_H

#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>

namespace UI {
    class InformationPanel final : public QWidget {
        Q_OBJECT

    public:
        explicit InformationPanel(QWidget* parent = nullptr);
        void createButtonBack();
        [[nodiscard]] QPushButton* getButtonBack() const;

    private:
        QHBoxLayout* layout_{nullptr};
        QPushButton* btnBack_{nullptr};
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_INFORMATIONPANEL_H
