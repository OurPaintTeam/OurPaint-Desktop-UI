#include "SettingsWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>

UI::SettingsWidget::SettingsWidget(QWidget *parent)
    : QWidget(parent), layout_(new QVBoxLayout(this))
{
    constexpr auto size = QSize(360, 220);
    setFixedSize(size);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_StyledBackground, true);

    constexpr auto sizeClB = QSize(28, 28);
    constexpr auto margins = 8;
    layout_->setContentsMargins(margins, margins, margins, margins);
    closeButton_ = new QPushButton("✕", this);
    closeButton_->setFixedSize(sizeClB);
    closeButton_->setFocusPolicy(Qt::NoFocus);
    closeButton_->setCursor(Qt::PointingHandCursor);

    connect(closeButton_, &QPushButton::clicked, this, &QWidget::close);

    layout_->addWidget(closeButton_, 0, Qt::AlignRight | Qt::AlignTop);

    auto *row1 = new QHBoxLayout();
    label1_ = new QLabel("Текст 1:", this);
    valueEdit1_ = new QLineEdit(this);
    valueEdit1_->setText("0.00");
    valueEdit1_->setFixedWidth(60);
    slider1_ = new QSlider(Qt::Horizontal, this);
    slider1_->setRange(0, 10000); // Диапазон 0–100.00 (умножаем на 100)
    slider1_->setValue(0);

    row1->addWidget(label1_);
    row1->addWidget(valueEdit1_);
    row1->addWidget(slider1_);

    auto *row2 = new QHBoxLayout();
    label2_ = new QLabel("Текст 2:", this);
    valueEdit2_ = new QLineEdit(this);
    valueEdit2_->setText("0.00");
    valueEdit2_->setFixedWidth(60);
    slider2_ = new QSlider(Qt::Horizontal, this);
    slider2_->setRange(0, 10000);
    slider2_->setValue(0);

    row2->addWidget(label2_);
    row2->addWidget(valueEdit2_);
    row2->addWidget(slider2_);

    auto *row3 = new QHBoxLayout();
    label3_ = new QLabel("Текст 3:", this);
    valueEdit3_ = new QLineEdit(this);
    valueEdit3_->setText("0.00");
    valueEdit3_->setFixedWidth(60);
    slider3_ = new QSlider(Qt::Horizontal, this);
    slider3_->setRange(0, 10000);
    slider3_->setValue(0);

    row3->addWidget(label3_);
    row3->addWidget(valueEdit3_);
    row3->addWidget(slider3_);

    layout_->addLayout(row1);
    layout_->addLayout(row2);
    layout_->addLayout(row3);

    applyButton_ = new QPushButton("Apply", this);
    applyButton_->setFixedSize(80, 30);
    applyButton_->setCursor(Qt::PointingHandCursor);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(applyButton_);

    layout_->addLayout(buttonLayout);

    connect(applyButton_, &QPushButton::clicked, this,
            &UI::SettingsWidget::onApplyClicked);

    connect(slider1_, &QSlider::valueChanged, [this](int value) {
        double doubleValue = value / 100.0; // Преобразуем 1234 → 12.34
        valueEdit1_->setText(QString::number(doubleValue, 'f', 2));
    });

    connect(slider2_, &QSlider::valueChanged, [this](int value) {
        double doubleValue = value / 100.0;
        valueEdit2_->setText(QString::number(doubleValue, 'f', 2));
    });

    connect(slider3_, &QSlider::valueChanged, [this](int value) {
        double doubleValue = value / 100.0;
        valueEdit3_->setText(QString::number(doubleValue, 'f', 2));
    });

    connect(valueEdit1_, &QLineEdit::textChanged, [this](const QString &text) {
        bool ok;
        double value = text.toDouble(&ok);
        if (ok) {
            slider1_->setValue(static_cast<int>(value * 100));
        }
    });

    connect(valueEdit2_, &QLineEdit::textChanged, [this](const QString &text) {
        bool ok;
        double value = text.toDouble(&ok);
        if (ok) {
            slider2_->setValue(static_cast<int>(value * 100));
        }
    });

    connect(valueEdit3_, &QLineEdit::textChanged, [this](const QString &text) {
        bool ok;
        double value = text.toDouble(&ok);
        if (ok) {
            slider3_->setValue(static_cast<int>(value * 100));
        }
    });

    layout_->addStretch();

    setObjectName(QStringLiteral("SettingsWidget"));
}

void UI::SettingsWidget::onApplyClicked()
{
    double value1 = valueEdit1_->text().toDouble();
    double value2 = valueEdit2_->text().toDouble();
    double value3 = valueEdit3_->text().toDouble();

    emit settingsApplied(value1, value2, value3);
    close();
}