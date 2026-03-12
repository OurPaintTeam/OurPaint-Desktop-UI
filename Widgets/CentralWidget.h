#ifndef OURPAINT_DESKTOP_UI_CENTRALWIDGET_H
#define OURPAINT_DESKTOP_UI_CENTRALWIDGET_H


#include <QWidget>

namespace UI {
    class CentralWidget final : public QWidget {
        Q_OBJECT

    public:
        explicit CentralWidget(QWidget* parent = nullptr);
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_CENTRALWIDGET_H
