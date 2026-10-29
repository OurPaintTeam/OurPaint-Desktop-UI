#ifndef OURPAINT_DESKTOP_UI_FRAMELESSWINDOW_H
#define OURPAINT_DESKTOP_UI_FRAMELESSWINDOW_H

#include <QMainWindow>

class QPoint;
class QMouseEvent;

namespace UI {

inline qreal DPR;

// Base window without system frame
class FramelessWindow : public QMainWindow {
    Q_OBJECT

protected:
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
    void resizeEvent(QResizeEvent* event) override;

    void changeEvent(QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    bool event(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void paintEvent(QPaintEvent* e) override;

public:
    explicit FramelessWindow(QWidget* parent = nullptr);
    ~FramelessWindow() override = default;

    virtual void initUI() = 0;

private:
#ifdef Q_OS_WIN
    void applyNativeWindowStyles() const;
#endif

    QPoint dragOffset_{};
    bool isDragging_{false};
    bool isStart{true};

    static QString loadStyle(const QString& path);
    static QString loadStyles(const QStringList& files);
    static void initTranslations();
    static void initFonts();

#ifdef Q_OS_WIN
private slots:
    void onViewportFrameReady();
#endif
};
}  // namespace UI

#endif  // OURPAINT_DESKTOP_UI_FRAMELESSWINDOW_H