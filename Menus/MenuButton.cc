#include "MenuButton.h"

#include <QMenu>
#include <QMouseEvent>
#include <QPainter>


namespace {
    constexpr int K_FONT_POINT_SIZE = 9;
    constexpr int K_CHECK_MARK_SIZE_PX = 4;
    constexpr int K_CHECK_MARK_RIGHT_MARGIN_PX = 6;
} // namespace

UI::MenuButton::MenuButton(const QString& text, QMenu* menu, QWidget* parent)
    : QPushButton(text, parent), menu_(menu) {
    setCheckable(true);
    setFocusPolicy(Qt::NoFocus);
    setAttribute(Qt::WA_Hover, true);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    QFont f;
    f.setPointSize(K_FONT_POINT_SIZE);
    f.setBold(true);
    setFont(f);

    if (menu) {
        setMenu(menu);

        connect(menu, &QMenu::aboutToShow, this, [this]() {
            setChecked(true);
        });
        connect(menu, &QMenu::aboutToHide, this, [this]() {
            setChecked(false);
        });
    }

    setObjectName(QStringLiteral("MenuButton"));
}


void UI::MenuButton::setInitialFromMenu() {
    if (!menu_) {
        return;
    }

    const auto actions = menu_->actions();
    if (actions.isEmpty()) {
        return;
    }
    applyAction(actions.first());
}


void UI::MenuButton::bindMenu(QMenu* menu) {
    menu_ = menu;
    if (!menu_) {
        return;
    }

    setMenu(menu_);

    for (auto* action: menu_->actions()) {
        connect(action, &QAction::triggered, this, [this, action]() {
            if (acceptChange_) {
                applyAction(action);
            }
        });
    }
}


void UI::MenuButton::setDraw(const bool draw) {
    draw_ = draw;
}


void UI::MenuButton::setOpenLeftClick(const bool acceptLeftClick) {
    openLeftClick_ = acceptLeftClick;
}


void UI::MenuButton::setChange(const bool acceptChange) {
    acceptChange_ = acceptChange;
}


void UI::MenuButton::paintEvent(QPaintEvent* event) {
    QPushButton::paintEvent(event);

    if (menu_ && draw_) {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(QColor(QStringLiteral("#D8D8F6")), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        const int w = width();
        const int h = height();

        const QPoint center(w - K_CHECK_MARK_RIGHT_MARGIN_PX - K_CHECK_MARK_SIZE_PX / 2, h / 2 + 1);

        const QPoint top(center.x() - K_CHECK_MARK_SIZE_PX, center.y() - K_CHECK_MARK_SIZE_PX / 4);
        const QPoint mid(center.x(), center.y() + K_CHECK_MARK_SIZE_PX / 3);
        const QPoint bottom(center.x() + K_CHECK_MARK_SIZE_PX, center.y() - K_CHECK_MARK_SIZE_PX / 4);

        p.drawLine(top, mid);
        p.drawLine(mid, bottom);
    }
}


void UI::MenuButton::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) {
        if (!openLeftClick_) {
            emit leftClickedTriggered(activeAction_);
            return;
        }

        if (!menu_) {
            return;
        }

        setChecked(true);

        const auto pos = mapToGlobal(QPoint(0, height()));
        menu_->exec(pos);

        setChecked(false);
        return;
    }

    if (e->button() == Qt::RightButton) {
        if (!menu_) {
            return;
        }

        setChecked(true);

        const auto pos = mapToGlobal(QPoint(0, height()));
        menu_->exec(pos);

        setChecked(false);
        return;
    }

    QPushButton::mousePressEvent(e);
}


void UI::MenuButton::wheelEvent(QWheelEvent* event) {
    if (!acceptChange_) {
        event->ignore();
        return;
    }

    if (!menu_ || menu_->actions().isEmpty()) {
        event->ignore();
        return;
    }

    const auto actions = menu_->actions();
    qsizetype index = actions.indexOf(activeAction_);

    if (index == -1) {
        index = 0;
    }

    const qsizetype count = actions.size();

    qsizetype blockStart = index;
    while (blockStart > 0 && !actions[blockStart - 1]->isSeparator()) {
        --blockStart;
    }

    qsizetype blockEnd = index;
    while (blockEnd < count - 1 && !actions[blockEnd + 1]->isSeparator()) {
        ++blockEnd;
    }

    if (event->angleDelta().y() > 0) {
        // вверх
        if (index > blockStart) {
            index--;
        } else {
            index = blockEnd;
        }
    } else {
        // вниз
        if (index < blockEnd) {
            index++;
        } else {
            index = blockStart;
        }
    }

    applyAction(actions[index]);
    emit leftClickedTriggered(activeAction_);

    event->accept();
}


void UI::MenuButton::applyAction(const QAction* action) {
    if (!action) {
        return;
    }

    if (!action->icon().isNull()) {
        setIcon(action->icon());
        setText(QString());
    } else {
        setIcon(QIcon());
        setText(action->text());
    }

    setToolTip(action->text());

    activeAction_ = action;
}
