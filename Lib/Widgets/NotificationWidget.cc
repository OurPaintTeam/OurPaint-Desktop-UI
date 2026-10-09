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

namespace {

class NotificationLabel final : public QLabel {
public:
    using QLabel::QLabel;

    QSize sizeHint() const override { return {240, heightForWidth(240)}; }

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
    void paintEvent(QPaintEvent* event) override {
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
        // Keep normal word breaks, but also wrap long paths and other unbroken text.
        option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        document.setDefaultTextOption(option);
        document.setPlainText(text());
        document.setTextWidth(qMax(1, width));
    }
};

}  // namespace

namespace UI {

NotificationWidget::NotificationWidget(const QString& text, QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, false);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layout_ = new QHBoxLayout(this);

    label_ = new NotificationLabel(this);
    closeButton_ = new QPushButton("✕", this);
    closeButton_->setFixedSize(20, 20);

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

    connect(closeButton_, &QPushButton::clicked, this, &NotificationWidget::onDeleteClicked);
}

void NotificationWidget::onDeleteClicked() { emit deleted(this); }

}  // namespace UI
