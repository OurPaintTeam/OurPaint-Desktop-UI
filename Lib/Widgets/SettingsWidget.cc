#include "SettingsWidget.h"
#include "SettingsSystem.h"

#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSlider>
#include <QVBoxLayout>

namespace {
    constexpr int kSliderScale = 100;
    constexpr int kSliderMax = 10000;

    QString formatStep(double v) { return QString::number(v, 'f', 2); }


    int languageToIndex(UI::SettingsSystem::Language lang) {
        switch (lang) {
            case UI::SettingsSystem::Language::Russian: return 1;
            case UI::SettingsSystem::Language::Belarusian: return 2;
            case UI::SettingsSystem::Language::English:
            default: return 0;
        }
    }


    UI::SettingsSystem::Language languageFromIndex(int index) {
        switch (index) {
            case 1: return UI::SettingsSystem::Language::Russian;
            case 2: return UI::SettingsSystem::Language::Belarusian;
            default: return UI::SettingsSystem::Language::English;
        }
    }
} // namespace

UI::SettingsWidget::SettingsWidget(SettingsSystem *settings, QWidget *parent)
    : QWidget(parent),
      layout_(new QVBoxLayout(this)),
      settings_(settings) {
    setFixedSize(400, 320);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName(QStringLiteral("SettingsWidget"));

    constexpr int margins = 8;
    layout_->setContentsMargins(margins, margins, margins, margins);

    auto *headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(0, 0, 0, 0);

    titleLabel_ = new QLabel(this);
    titleLabel_->setObjectName("SettingsTitle");

    QFont font = titleLabel_->font();
    font.setPointSize(14);
    font.setBold(true);
    titleLabel_->setFont(font);

    closeButton_ = new QPushButton(QStringLiteral("✕"), this);
    closeButton_->setFixedSize(28, 28);
    closeButton_->setFocusPolicy(Qt::NoFocus);
    closeButton_->setCursor(Qt::PointingHandCursor);
    connect(closeButton_, &QPushButton::clicked, this, &QWidget::close);

    headerLayout->addWidget(titleLabel_);
    headerLayout->addStretch();
    headerLayout->addWidget(closeButton_);

    layout_->addLayout(headerLayout);

    gridCheck_ = new QCheckBox(this);
    layout_->addWidget(gridCheck_);

    auto *gridRow = new QHBoxLayout();
    gridStepLabel_ = new QLabel(this);
    gridStepEdit_ = new QLineEdit(this);
    gridStepEdit_->setFixedWidth(70);
    gridStepSlider_ = new QSlider(Qt::Horizontal, this);
    gridStepSlider_->setRange(0, kSliderMax);
    gridRow->addWidget(gridStepLabel_);
    gridRow->addWidget(gridStepEdit_);
    gridRow->addWidget(gridStepSlider_);
    layout_->addLayout(gridRow);

    snapCheck_ = new QCheckBox(this);
    layout_->addWidget(snapCheck_);

    auto *snapRow = new QHBoxLayout();
    snapStepLabel_ = new QLabel(this);
    snapStepEdit_ = new QLineEdit(this);
    snapStepEdit_->setFixedWidth(70);
    snapStepSlider_ = new QSlider(Qt::Horizontal, this);
    snapStepSlider_->setRange(0, kSliderMax);
    snapRow->addWidget(snapStepLabel_);
    snapRow->addWidget(snapStepEdit_);
    snapRow->addWidget(snapStepSlider_);
    layout_->addLayout(snapRow);

    auto *langRow = new QHBoxLayout();
    languageLabel_ = new QLabel(this);
    languageCombo_ = new QComboBox(this);
    langRow->addWidget(languageLabel_);
    langRow->addWidget(languageCombo_, 1);
    layout_->addLayout(langRow);

    auto *notifRow = new QHBoxLayout();
    notificationsCheck_ = new QCheckBox(this);
    notifRow->addWidget(notificationsCheck_);
    layout_->addLayout(notifRow);

    layout_->addStretch();

    wireSignals();
    translate();
    loadFromSettings();
}


void UI::SettingsWidget::wireSignals() {
    connect(gridCheck_, &QCheckBox::toggled, this, [this](bool on) {
        if (settings_) settings_->setGridEnabled(on);
        emit settingsChanged();
    });

    connect(snapCheck_, &QCheckBox::toggled, this, [this](bool on) {
        if (settings_) settings_->setSnapEnabled(on);
        emit settingsChanged();
    });

    connect(gridStepSlider_, &QSlider::valueChanged, this, [this](int v) {
        const QSignalBlocker blocker(gridStepEdit_);
        gridStepEdit_->setText(formatStep(v / double(kSliderScale)));
        commitGridStep(v / double(kSliderScale));
    });
    connect(notificationsCheck_, &QCheckBox::toggled, this, [this](bool on) {
       if (settings_) settings_->setNotificationsEnabled(on);
       emit settingsChanged();
   });
    connect(gridStepEdit_, &QLineEdit::textEdited, this, [this](const QString& text) {
        bool ok = false;
        const double v = text.toDouble(&ok);
        if (!ok) return;
        const QSignalBlocker blocker(gridStepSlider_);
        gridStepSlider_->setValue(qBound(0, int(v * kSliderScale), kSliderMax));
        commitGridStep(v);
    });

    connect(snapStepSlider_, &QSlider::valueChanged, this, [this](int v) {
        const QSignalBlocker blocker(snapStepEdit_);
        snapStepEdit_->setText(formatStep(v / double(kSliderScale)));
        commitSnapStep(v / double(kSliderScale));
    });
    connect(snapStepEdit_, &QLineEdit::textEdited, this, [this](const QString& text) {
        bool ok = false;
        const double v = text.toDouble(&ok);
        if (!ok) return;
        const QSignalBlocker blocker(snapStepSlider_);
        snapStepSlider_->setValue(qBound(0, int(v * kSliderScale), kSliderMax));
        commitSnapStep(v);
    });

    connect(languageCombo_, &QComboBox::currentIndexChanged, this, [this](int idx) {
        if (settings_) settings_->setLanguage(languageFromIndex(idx));
        emit settingsChanged();
    });
}


void UI::SettingsWidget::commitGridStep(double value) {
    if (settings_) settings_->setGridStep(value);
    emit settingsChanged();
}


void UI::SettingsWidget::commitSnapStep(double value) {
    if (settings_) settings_->setSnapStep(value);
    emit settingsChanged();
}

bool UI::SettingsSystem::isNotificationsEnabled() const {
    return settings_.value("notificationsEnabled", true).toBool();
}

void UI::SettingsSystem::setNotificationsEnabled(bool enabled) {
    settings_.setValue("notificationsEnabled", enabled);
}

void UI::SettingsWidget::loadFromSettings() {
    if (!settings_) return;

    const QSignalBlocker b1(gridCheck_);
    const QSignalBlocker b2(snapCheck_);
    const QSignalBlocker b3(gridStepEdit_);
    const QSignalBlocker b4(gridStepSlider_);
    const QSignalBlocker b5(snapStepEdit_);
    const QSignalBlocker b6(snapStepSlider_);
    const QSignalBlocker b7(languageCombo_);
    const QSignalBlocker b8(notificationsCheck_);

    notificationsCheck_->setChecked(settings_->isNotificationsEnabled());

    gridCheck_->setChecked(settings_->isGridEnabled());
    snapCheck_->setChecked(settings_->isSnapEnabled());

    const double g = settings_->gridStep();
    gridStepEdit_->setText(formatStep(g));
    gridStepSlider_->setValue(qBound(0, int(g * kSliderScale), kSliderMax));

    const double s = settings_->snapStep();
    snapStepEdit_->setText(formatStep(s));
    snapStepSlider_->setValue(qBound(0, int(s * kSliderScale), kSliderMax));

    if (languageCombo_->count() >= 3)
        languageCombo_->setCurrentIndex(languageToIndex(settings_->language()));
}


void UI::SettingsWidget::closeEvent(QCloseEvent *e) {
    if (settings_) settings_->save();
    QWidget::closeEvent(e);
}


void UI::SettingsWidget::changeEvent(QEvent *e) {
    if (e && e->type() == QEvent::LanguageChange)
        translate();
    QWidget::changeEvent(e);
}


void UI::SettingsWidget::translate() const {
    if (titleLabel_) titleLabel_->setText(tr("Settings"));
    if (closeButton_) closeButton_->setToolTip(tr("Close"));

    if (gridCheck_) gridCheck_->setText(tr("Show grid"));
    if (gridStepLabel_) gridStepLabel_->setText(tr("Grid step:"));
    if (gridStepEdit_) gridStepEdit_->setToolTip(tr("Grid step"));
    if (gridStepSlider_) gridStepSlider_->setToolTip(tr("Grid step"));

    if (snapCheck_) snapCheck_->setText(tr("Enable snapping"));
    if (snapStepLabel_) snapStepLabel_->setText(tr("Snap step:"));
    if (snapStepEdit_) snapStepEdit_->setToolTip(tr("Snap step"));
    if (snapStepSlider_) snapStepSlider_->setToolTip(tr("Snap step"));

    if (languageLabel_) languageLabel_->setText(tr("Language:"));

    if (languageCombo_) {
        const int current = languageCombo_->currentIndex();
        const QSignalBlocker blocker(languageCombo_);
        languageCombo_->clear();
        languageCombo_->addItem(tr("English"));
        languageCombo_->addItem(tr("Russian"));
        languageCombo_->addItem(tr("Belarusian"));
        languageCombo_->setCurrentIndex(current >= 0 ? current : 0);
    }

    if (notificationsCheck_) notificationsCheck_->setText(tr("Show notifications"));
}
