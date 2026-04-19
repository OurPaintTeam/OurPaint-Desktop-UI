#include "TabWidget.h"

#include <QApplication>
#include <QDrag>
#include <QHBoxLayout>
#include <QMimeData>
#include <QMouseEvent>
#include <QPushButton>
#include <QStyle>
#include <QToolButton>
#include <QLineEdit>

#include "RenameTabLineEdit.h"


namespace {
    constexpr int MAX_TAB_TEXT_WIDTH = 120;
    constexpr int K_TAB_HORIZONTAL_PADDING_PX = 8;
    constexpr int K_TAB_LAYOUT_SPACING_PX = 4;
    constexpr int K_CLOSE_BUTTON_WIDTH_PX = 16;
    constexpr auto K_TAB_MIME_TYPE = "application/x-tabwidget";
} // namespace


UI::TabWidget::TabWidget(QWidget* parent,const QString& name)
    : QWidget(parent)
      , nameButton_(new QPushButton(this)), closeButton_(new QToolButton(this)), layout_(new QHBoxLayout(this)),
      name_(name), edit_(new RenameTabLineEdit(this)) {
    setAttribute(Qt::WA_StyledBackground, true);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    layout_->setContentsMargins(K_TAB_HORIZONTAL_PADDING_PX, 0, K_TAB_HORIZONTAL_PADDING_PX, 0);
    layout_->setSpacing(K_TAB_LAYOUT_SPACING_PX);
    nameButton_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    setName(name);

    closeButton_->setText("✕");
    closeButton_->setFixedWidth(K_CLOSE_BUTTON_WIDTH_PX);
    closeButton_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    layout_->addWidget(nameButton_);
    layout_->addWidget(closeButton_);

    applyInactiveStyle();

    connect(nameButton_, &QPushButton::clicked, this, [this]() {
        emit clickedTriggered(this);
    });

    connect(closeButton_, &QToolButton::clicked, this, [this]() {
        emit closeRequestedTriggered(this);
    });

  edit_->hide();
  edit_->setText(name_);
  edit_->setFrame(false);
    edit_->setObjectName("TabLineEdit");



  layout_->insertWidget(0, edit_);

  connect(edit_, &QLineEdit::returnPressed, this, &TabWidget::finishRename);

    connect(edit_, &RenameTabLineEdit::focusLost, this,&TabWidget::cancelRename);

  nameButton_->installEventFilter(this);
    setObjectName(QStringLiteral("TabWidgetActive"));
}


bool UI::TabWidget::eventFilter(QObject* obj, QEvent* event)
{
  if (obj == nameButton_) {
    if (event->type() == QEvent::MouseButtonDblClick) {
      startRename();
      return true;
    }
  }

  return QWidget::eventFilter(obj, event);
}

QString UI::TabWidget::getName() const {
    return name_;
}


void UI::TabWidget::setName(const QString& name) {
    name_ = name;

    const QString displayText = elideText(name_, MAX_TAB_TEXT_WIDTH);
    nameButton_->setText(displayText);
    nameButton_->setToolTip(name_);
}


void UI::TabWidget::setActive(const bool active) {
    active ? applyActiveStyle() : applyInactiveStyle();
}


void UI::TabWidget::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) {
        dragStartPos_ = e->pos();
    }

  if (e->button() == Qt::RightButton) {
    emit rightClickedTriggered(this, e->pos());
    return;
  }

    QWidget::mousePressEvent(e);
}


void UI::TabWidget::mouseMoveEvent(QMouseEvent* e) {
    if (!(e->buttons() & Qt::LeftButton)) {
        return;
    }

    if ((e->pos() - dragStartPos_).manhattanLength()
        < QApplication::startDragDistance()) {
        return;
    }

    auto drag = std::make_unique<QDrag>(this);
    auto mime = std::make_unique<QMimeData>();

    mime->setData(K_TAB_MIME_TYPE, QByteArray::number(reinterpret_cast<quintptr>(this)));
    mime->setData("application/x-window-id",
              QByteArray::number(reinterpret_cast<quintptr>(window())));
    drag->setMimeData(mime.release());

    if (const auto action = drag->exec(Qt::MoveAction);
        action == Qt::IgnoreAction) {
        emit draggedOutTriggered(this);
    }
}


void UI::TabWidget::applyInactiveStyle() {
    nameButton_->setObjectName("NameButtonTabInActive");
    closeButton_->setObjectName("CloseButtonTabInActive");
    setObjectName(QStringLiteral("TabWidgetInActive"));
    cancelRename();

    style()->unpolish(this);
    style()->polish(this);
    update();
}


void UI::TabWidget::applyActiveStyle() {
    nameButton_->setObjectName("NameButtonTabActive");
    closeButton_->setObjectName("CloseButtonTabActive");
    setObjectName(QStringLiteral("TabWidgetActive"));

    style()->unpolish(this);
    style()->polish(this);
    update();
}


void UI::TabWidget::startRename()
{
  edit_->setText(name_);
  nameButton_->hide();
    edit_->show();

  edit_->setFocus();
  edit_->selectAll();
}

void UI::TabWidget::finishRename()
{
  const QString newName = edit_->text().trimmed();

  if (!newName.isEmpty() && newName != name_) {
    emit renameTriggered(name_, newName);
  }

  edit_->hide();
  nameButton_->show();
}


void UI::TabWidget::cancelRename()
{
  if (edit_) {
    edit_->hide();
  }
  nameButton_->show();
    emit cancelRenameTriggered();
}

QString UI::TabWidget::elideText(const QString& text, int maxWidth) const
{
    QFontMetrics fm(nameButton_->font());
    return fm.elidedText(text, Qt::ElideRight, maxWidth);
}