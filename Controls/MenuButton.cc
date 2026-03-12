#include "MenuButton.h"

#include <QMouseEvent>
#include <QPainter>


#include "ArcMenu.h"

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

    QFont f;
    f.setPointSize(K_FONT_POINT_SIZE);
    f.setBold(true);
    setFont(f);

    setStyleSheet(R"(
    QPushButton {
        background: transparent;
        color: #D8D8F6;
        border: none;
        text-align: center;
        padding: 4px 10px;
        border-radius: 6px;
    }

    QPushButton::menu-indicator {
        image: none;          /* убираем стрелку */
    }

    QPushButton:hover {
        background: rgba(255,255,255,0.08);
    }

    QPushButton:pressed,
    QPushButton:checked {
        background: rgba(255,255,255,0.16);
    }
)");

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

    if (const auto* arcMenu = qobject_cast<ArcMenu*>(menu_)) {
        if (const QAction* firstAction = arcMenu->defaultAction()) {
            setText(firstAction->text());
        }
    } else {
        if (auto actions = menu_->actions(); !actions.isEmpty()) {
            setText(actions.first()->text());
        }
    }
}


void UI::MenuButton::bindMenu(QMenu* menu) {
    menu_ = menu;
    setMenu(menu_);

    if (const auto* arcMenu = qobject_cast<UI::ArcMenu*>(menu_)) {
        if (arcMenu->defaultAction()) {
            setText(arcMenu->defaultAction()->text());
        }

        connect(arcMenu, &UI::ArcMenu::actionSelected,
                this, &UI::MenuButton::onArcActionSelected);
    }
}


void UI::MenuButton::setDraw(const bool draw) {
    draw_ = draw;
}


void UI::MenuButton::setLeftClick(const bool acceptLeftClick) {
    acceptLeftClick_ = acceptLeftClick;
}

void UI::MenuButton::setRightClick(const bool acceptRightClick) {
    acceptRightClick_ = acceptRightClick;
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
    if ((e->button() == Qt::LeftButton && !acceptLeftClick_) ||
        (e->button() == Qt::RightButton && !acceptRightClick_)) {
        QPushButton::mousePressEvent(e);
        return;
    }

    QPushButton::mousePressEvent(e);

    if (!menu_) {
        return;
    }

    setChecked(true);

    const auto pos = mapToGlobal(QPoint(0, height()));
    menu_->exec(pos);

    setChecked(false);
}


void UI::MenuButton::onArcActionSelected(const QAction* action)
{
    setText(action->text());
}
