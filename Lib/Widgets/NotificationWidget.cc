#include "NotificationWidget.h"
#include <QAbstractTextDocumentLayout>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QTextDocument>
#include <QTextOption>
#include <QtMath>
#include <QTimer>
#include <QEvent>
#include <QFile>

namespace {
    class NotificationLabel final : public QLabel {
    public:
        using QLabel::QLabel;

        QSize sizeHint() const override {
            return {240, heightForWidth(240)};
        }

        QSize minimumSizeHint() const override {
            ensurePolished();
            const auto margins = contentsMargins();
            return {0, fontMetrics().height() + margins.top() + margins.bottom()};
        }

        int heightForWidth(int width) const override {
            ensurePolished();
            const auto margins = contentsMargins();
            QTextDocument document;
            layoutText(document, width - margins.left() - margins.right());
            return qCeil(document.size().height()) + margins.top() + margins.bottom();
        }

    protected:
        void paintEvent(QPaintEvent *event) override {
            QFrame::paintEvent(event);
            QPainter painter(this);
            painter.setClipRect(contentsRect());
            painter.translate(contentsRect().topLeft());

            QTextDocument document;
            layoutText(document, contentsRect().width());

            QAbstractTextDocumentLayout::PaintContext context;
            context.palette = palette();
            context.palette.setColor(QPalette::Text, palette().color(foregroundRole()));

            document.documentLayout()->draw(&painter, context);
        }

    private:
        void layoutText(QTextDocument& document, int width) const {
            document.setDocumentMargin(0);
            document.setDefaultFont(font());
            QTextOption option;
            option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
            document.setDefaultTextOption(option);
            document.setPlainText(text());
            document.setTextWidth(qMax(1, width));
        }
    };
}

UI::NotificationWidget::NotificationWidget(const QString& text, QWidget *anchorWidget, QWidget *parent)
    : QFrame(parent), anchorWidget_(anchorWidget) {

    setAttribute(Qt::WA_DeleteOnClose, true);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setFrameShape(QFrame::NoFrame);

    QFile styleFile(":/Styles/notif.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        setStyleSheet(QString::fromUtf8(styleFile.readAll()));
    } else {
        qWarning() << "Failed to load style for NotificationWidget!";
    }

    setFixedWidth(320);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    layout_ = new QHBoxLayout(this);
    layout_->setContentsMargins(10, 10, 10, 10);

    label_ = new NotificationLabel(this);
    closeButton_ = new QPushButton("✕", this);
    closeButton_->setFixedSize(20, 20);
    closeButton_->setCursor(Qt::PointingHandCursor);

    label_->setTextFormat(Qt::PlainText);
    label_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    label_->setWordWrap(true);
    label_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    label_->setText(text);

    layout_->addWidget(label_, 1);
    layout_->addWidget(closeButton_, 0, Qt::AlignTop);

    setObjectName("NotificationWidget");
    label_->setObjectName("NotificationWidgetLabel");
    closeButton_->setObjectName("NotificationWidgetClose");

    connect(closeButton_, &QPushButton::clicked, this, &NotificationWidget::onCloseClicked);

    hideTimer_ = new QTimer(this);
    hideTimer_->setSingleShot(true);
    connect(hideTimer_, &QTimer::timeout, this, &NotificationWidget::onCloseClicked);

    adjustSize();

    if (anchorWidget_) {
        anchorWidget_->installEventFilter(this);
    }
}

void UI::NotificationWidget::showFor(int milliseconds) {
    show();
    updatePosition();
    hideTimer_->start(milliseconds);
}

void UI::NotificationWidget::updatePosition() {
    if (!anchorWidget_ || !anchorWidget_->isVisible()) {
        hide();
        return;
    }

    constexpr int margin = 20;
    const auto bottomRight = anchorWidget_->mapToGlobal(anchorWidget_->rect().bottomRight());
    move(bottomRight.x() - width() - margin, bottomRight.y() - height() - margin);
}

bool UI::NotificationWidget::eventFilter(QObject *obj, QEvent *event) {
    if (obj == anchorWidget_) {
        switch (event->type()) {
            case QEvent::Move:
            case QEvent::Resize:
                updatePosition();
                break;
            case QEvent::Hide:
                hide();
                break;
            case QEvent::Close:
                close();
                break;
            default:
                break;
        }
    }
    return QFrame::eventFilter(obj, event);
}

void UI::NotificationWidget::enterEvent(QEnterEvent *event) {
    QWidget::enterEvent(event);
    hideTimer_->stop();
}

void UI::NotificationWidget::leaveEvent(QEvent *event) {
    QWidget::leaveEvent(event);
    hideTimer_->start(3000);
}

void UI::NotificationWidget::onCloseClicked() {
    hideTimer_->stop();
    emit finished();
    close();
}