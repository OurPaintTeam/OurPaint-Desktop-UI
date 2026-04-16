#ifndef OURPAINT_DESKTOP_UI_TOPBARSTART_H
#define OURPAINT_DESKTOP_UI_TOPBARSTART_H

class QLabel;

#include "TopBar.h"

namespace UI {
    class TopBarStart final : public TopBar {
        Q_OBJECT

    public:
        explicit TopBarStart(QWidget* parent = nullptr);

    private:
        QLabel* title_{nullptr};
    };
} // namespace UI

#endif //OURPAINT_DESKTOP_UI_TOPBARSTART_H
