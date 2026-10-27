#include "FramelessWindow.h"

#include <qdir.h>
#include <qfontdatabase.h>

#ifdef Q_OS_WIN

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>

namespace {

constexpr DWORD kDwmWindowCornerPreference  = 33;
constexpr DWORD kDwmBorderColor             = 34;
constexpr DWORD kDwmCaptionColor            = 35;

enum class WindowCornerPreference : DWORD {
    Default    = 0,
    DoNotRound = 1,
    Round      = 2,
    RoundSmall = 3
};

} // namespace

#endif

#include <QApplication>
#include <QFile>
#include <QMouseEvent>
#include <QTranslator>
#include <QEvent>
#include <QScreen>
#include <QWindow>

UI::FramelessWindow::FramelessWindow(QWidget *parent) : QMainWindow(parent) {
    #ifdef Q_OS_LINUX
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    qApp->installEventFilter(this);
    #endif
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setObjectName(QStringLiteral("FramelessWindow"));
    setWindowTitle(tr("OurPaint"));
    setWindowIcon(QIcon(":/Assets/logo/logo2.ico"));

    const auto style = loadStyles({
        ":/Styles/app.qss", ":/Styles/buttons.qss",
        ":/Styles/menu.qss", ":/Styles/tooltip.qss",
        ":/Styles/window.qss", ":/Styles/text.qss",
        ":/Styles/scroll.qss"
    });

    if (style.isEmpty()) {
        qWarning() << "No styles loaded!";
    } else {
        setStyleSheet(style);
    }

    initTranslations();
    initFonts();

#ifdef Q_OS_WIN
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint); /* отключает заголовок*/
    applyNativeWindowStyles();
    initWindowForWindowsFrame();
    // initWindowForWindowsWithoutFrame();
#endif

    constexpr int sizeW = 960;
    constexpr int sizeY = 540;
    constexpr auto size = QSize(sizeW, sizeY);
    resize(size);
}
#ifdef Q_OS_LINUX
bool UI::FramelessWindow::eventFilter(QObject* watched, QEvent* event){
    if (event->type() == QEvent::MouseButtonPress && !isMaximized() && !isFullScreen()){
        auto* widget = qobject_cast<QWidget*>(watched);
        auto* mouse = static_cast<QMouseEvent*>(event);
        if (widget && widget->window() == this && mouse->button() == Qt::LeftButton){
            constexpr int borderWidth = 8;
            const QPoint pos = mapFromGlobal(mouse->globalPosition().toPoint());
            Qt::Edges edges;
            if (pos.x() < borderWidth) edges |= Qt::LeftEdge;
            else if (pos.x() >= width() - borderWidth) edges |= Qt::RightEdge;
            if (pos.y() < borderWidth) edges |= Qt::TopEdge;
            else if (pos.y() >= width() - borderWidth) edges |= Qt::BottomEdge;
            
            if (edges && windowHandle()->startSystemResize(edges)) return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}
#endif

#ifdef Q_OS_WIN
void UI::FramelessWindow::applyNativeWindowStyles() {
    HWND hwnd = reinterpret_cast<HWND>(winId());
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);

    // Сохраняем/устанавливаем стили, которые обычно удаляет FramelessWindowHint
    style |= WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    SetWindowLongPtr(hwnd, GWL_STYLE, style);

    // Принудительно применяем изменения
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
}
#endif

#ifdef Q_OS_WIN
void UI::FramelessWindow::initWindowForWindowsWithoutFrame() const {
    HWND hwnd = reinterpret_cast<HWND>(winId());

    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    style &= ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME);
    SetWindowLongPtr(hwnd, GWL_STYLE, style);

    const auto cornerPreference = WindowCornerPreference::Round;

    const HRESULT result = DwmSetWindowAttribute(
        hwnd,
        kDwmWindowCornerPreference,
        &cornerPreference,
        sizeof(cornerPreference)
    );

    if (FAILED(result)) {
        qWarning() << "Failed to set window corner preference:" << Qt::hex << result;
    }
}


void UI::FramelessWindow::initWindowForWindowsFrame() const {
    HWND hwnd = reinterpret_cast<HWND>(winId());
    const COLORREF captionColor = RGB(73, 72, 80); // #494850
   // const COLORREF captionColor = RGB(31, 52, 56);

    HRESULT result = DwmSetWindowAttribute(
        hwnd,
        kDwmCaptionColor,
        &captionColor,
        sizeof(captionColor)
    );

    if (FAILED(result)) {
        qWarning() << "Failed to set caption color:"
                   << Qt::hex << result;
    }

    const COLORREF borderColor = RGB(95, 94, 105); // #5f5e69
   // const COLORREF borderColor = RGB(73, 72, 80);

    result = DwmSetWindowAttribute(
        hwnd,
        kDwmBorderColor,
        &borderColor,
        sizeof(borderColor)
    );

    if (FAILED(result)) {
        qWarning() << "Failed to set border color:"
                   << Qt::hex << result;
    }
}


void UI::FramelessWindow::updateWindowCorners() const {
    HWND hwnd = reinterpret_cast<HWND>(winId());

    const auto preference =
        (isMaximized() || isFullScreen())
            ? WindowCornerPreference::DoNotRound
            : WindowCornerPreference::Round;

    const HRESULT result = DwmSetWindowAttribute(
        hwnd,
        kDwmWindowCornerPreference,
        &preference,
        sizeof(preference)
    );

    if (FAILED(result)) {
        qWarning() << "Failed to update window corner preference:"
                   << Qt::hex << result;
    }
}
#endif

bool UI::FramelessWindow::nativeEvent(const QByteArray& eventType,
                                      void* message, qintptr* result) {
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG") {
        MSG* msg = static_cast<MSG*>(message);

        switch (msg->message) {
            case WM_NCCALCSIZE: {
                if (msg->wParam == TRUE) {
                    // Для максимизированного окна нужно скорректировать область,
                    // чтобы она не выходила за пределы рабочего стола.
                    if (isMaximized()) {
                        auto* params = reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
                        MONITORINFO mi{ sizeof(MONITORINFO) };
                        if (GetMonitorInfo(MonitorFromWindow(msg->hwnd, MONITOR_DEFAULTTONEAREST), &mi)) {
                            params->rgrc[0] = mi.rcWork;
                        }
                    }
                    *result = 0;
                    return true; // Сообщаем, что мы обработали сообщение
                }
                break;
            }
            case WM_NCHITTEST: {
                const LONG borderWidth = 8; // Ширина зоны захвата
                RECT winrect;
                GetWindowRect(msg->hwnd, &winrect);

                long x = GET_X_LPARAM(msg->lParam);
                long y = GET_Y_LPARAM(msg->lParam);

                // Проверка на края (как в вашем закомментированном коде)
                bool left = x < winrect.left + borderWidth;
                bool right = x > winrect.right - borderWidth;
                bool top = y < winrect.top + borderWidth;
                bool bottom = y > winrect.bottom - borderWidth;

                if (top && left) { *result = HTTOPLEFT; return true; }
                if (top && right) { *result = HTTOPRIGHT; return true; }
                if (bottom && left) { *result = HTBOTTOMLEFT; return true; }
                if (bottom && right) { *result = HTBOTTOMRIGHT; return true; }
                if (left) { *result = HTLEFT; return true; }
                if (right) { *result = HTRIGHT; return true; }
                if (top) { *result = HTTOP; return true; }
                if (bottom) { *result = HTBOTTOM; return true; }

                // Проверка на область заголовка (drag)
                // Преобразуем координаты в клиентские
                POINT clientPoint{ x, y };
                ScreenToClient(msg->hwnd, &clientPoint);

                // Если Y меньше высоты вашего кастомного заголовка
                if (clientPoint.y < 30) { // titleBarHeight() - ваша функция
                    *result = HTCAPTION;
                    return true;
                }

                break; // Для клиентской области позволяем Qt обработать дальше
            }
            case WM_GETMINMAXINFO: {
                // Код из вашего ImGui-примера
                auto* mmi = reinterpret_cast<MINMAXINFO*>(msg->lParam);
                HMONITOR hMon = MonitorFromWindow(msg->hwnd, MONITOR_DEFAULTTONEAREST);
                MONITORINFO mi{ sizeof(MONITORINFO) };
                if (GetMonitorInfoW(hMon, &mi)) {
                    mmi->ptMaxPosition.x = mi.rcWork.left - mi.rcMonitor.left;
                    mmi->ptMaxPosition.y = mi.rcWork.top - mi.rcMonitor.top;
                    mmi->ptMaxSize.x = mi.rcWork.right - mi.rcWork.left;
                    mmi->ptMaxSize.y = mi.rcWork.bottom - mi.rcWork.top;
                }
                *result = 0;
                return true;
            }
            // ... обработка других сообщений ...
        }
    }
#endif
    return QMainWindow::nativeEvent(eventType, message, result);
}


void UI::FramelessWindow::changeEvent(QEvent *event) {
#ifdef Q_OS_WIN
    if (event->type() == QEvent::LanguageChange) {
        setWindowTitle(tr("OurPaint"));
        update();
    }

    if (event->type() == QEvent::WindowStateChange) {
        /*
         * Если initWindowForWindowsWithoutFrame()
         */
        //   updateWindowCorners();
    }
#endif
    QMainWindow::changeEvent(event);
}


QString UI::FramelessWindow::loadStyle(const QString& path) {
    QFile file(path);
    if (!file.exists()) {
        qWarning() << "Resource not found:" << path;
        return "";
    }

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Failed to open style file:" << path;
        return "";
    }

    return QString::fromUtf8(file.readAll());
}


QString UI::FramelessWindow::loadStyles(const QStringList& files) {
    QString style;
    for (const auto& file: files) {
        style += loadStyle(file);
    }
    return style;
}


void UI::FramelessWindow::showEvent(QShowEvent *event) {
    if (isStart) {
        QRect screenGeometry =
                QGuiApplication::primaryScreen()->availableGeometry();
        move(screenGeometry.center() - rect().center());
        isStart = false;
    }
    QMainWindow::showEvent(event);
}


void UI::FramelessWindow::initFonts() {
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    int id1 = QFontDatabase::addApplicationFont(":/Styles/font/static/Inter-Regular.ttf");
    int id2 = QFontDatabase::addApplicationFont(":/Styles/font/static/Inter-Medium.ttf");
    int id3 = QFontDatabase::addApplicationFont(":/Styles/font/static/Inter-SemiBold.ttf");

    if (id1 == -1 || id2 == -1 || id3 == -1) {
        qDebug() << "Failed to load one of the fonts!";
    }

    QString family = QFontDatabase::applicationFontFamilies(id1).value(0);

    if (family.isEmpty()) {
        qDebug() << "Font family is empty!";
        return;
    }

    QFont font(family);

    font.setHintingPreference(QFont::PreferFullHinting);
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setStyleStrategy(QFont::PreferQuality);

    // Применяем
    qApp->setFont(font);
}


void UI::FramelessWindow::initTranslations() {
    /*
* lupdate . -ts Lib/translations/app_ru.ts
* lrelease Lib/translations/app_ru.ts
  */

    static QTranslator translator;
    const bool ok = translator.load(":/translations/app_eng.qm");
    qApp->installTranslator(&translator);

    if (!ok) {
        qWarning() << "No translate files";
    }
}
