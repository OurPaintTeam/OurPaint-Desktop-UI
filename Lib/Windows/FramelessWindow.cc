#include "FramelessWindow.h"

#include <qdir.h>
#include <qfontdatabase.h>

#include <QLayout>
#include <QPushButton>
#include <QTimer>

#include "TopBar.h"

#ifdef Q_OS_WIN

#include <dwmapi.h>
#include <windows.h>
#include <windowsx.h>


namespace {

constexpr DWORD kDwmWindowCornerPreference = 33;
constexpr DWORD kDwmBorderColor = 34;
constexpr DWORD kDwmCaptionColor = 35;

enum class WindowCornerPreference : DWORD { Default = 0, DoNotRound = 1, Round = 2, RoundSmall = 3 };

}  // namespace

#endif

#include <QApplication>
#include <QEvent>
#include <QFile>
#include <QMouseEvent>
#include <QScreen>
#include <QTranslator>
#include <QWindow>

UI::FramelessWindow::FramelessWindow(QWidget* parent) : QMainWindow(parent) {
#ifdef Q_OS_WIN
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::ExpandedClientAreaHint | Qt::NoTitleBarBackgroundHint);
#else
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    setAttribute(Qt::WA_TranslucentBackground);
    qApp->installEventFilter(this);
#endif

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);

    setObjectName(QStringLiteral("FramelessWindow"));
    setWindowTitle(tr("OurPaint"));
    setWindowIcon(QIcon(":/Assets/logo/logo2.ico"));

    const auto style = loadStyles({":/Styles/app.qss", ":/Styles/buttons.qss", ":/Styles/menu.qss", ":/Styles/tooltip.qss", ":/Styles/window.qss",
                                   ":/Styles/text.qss", ":/Styles/scroll.qss"});

    if (style.isEmpty()) {
        qWarning() << "No styles loaded!";
    } else {
        setStyleSheet(style);
    }

    initTranslations();
    initFonts();

    setMinimumSize(900, 300);
    resize(960, 540);

    installEventFilter(this);
}

bool UI::FramelessWindow::eventFilter(QObject* watched, QEvent* event) {
#ifdef Q_OS_LINUX
    if (event->type() == QEvent::MouseButtonPress && !isMaximized() && !isFullScreen()) {
        auto* widget = qobject_cast<QWidget*>(watched);
        auto* mouse = static_cast<QMouseEvent*>(event);
        if (widget && widget->window() == this && mouse->button() == Qt::LeftButton) {
            constexpr int borderWidth = 8;
            const QPoint pos = mapFromGlobal(mouse->globalPosition().toPoint());
            Qt::Edges edges;
            if (pos.x() < borderWidth) {
                edges |= Qt::LeftEdge;
            } else if (pos.x() >= width() - borderWidth) {
                edges |= Qt::RightEdge;
            }
            if (pos.y() < borderWidth) {
                edges |= Qt::TopEdge;
            } else if (pos.y() >= width() - borderWidth) {
                edges |= Qt::BottomEdge;
            }

            if (edges && windowHandle()->startSystemResize(edges)) {
                return true;
            }
        }
    }
#endif

    return QMainWindow::eventFilter(watched, event);
}


#ifdef Q_OS_WIN
void UI::FramelessWindow::applyNativeWindowStyles() const {

    HWND hwnd = reinterpret_cast<HWND>(winId());

    const COLORREF captionColor = RGB(73, 72, 80);

    HRESULT result = DwmSetWindowAttribute(hwnd, kDwmCaptionColor, &captionColor, sizeof(captionColor));

    if (FAILED(result)) {
        qWarning() << "Failed to set caption color:" << Qt::hex << result;
    }

    const COLORREF borderColor = RGB(73, 72, 80);

    result = DwmSetWindowAttribute(hwnd, kDwmBorderColor, &borderColor, sizeof(borderColor));

    if (FAILED(result)) {
        qWarning() << "Failed to set border color:" << Qt::hex << result;
    }


    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);

    style &= ~WS_CAPTION;
    style |= WS_THICKFRAME | WS_MAXIMIZEBOX;

    SetWindowLongPtr(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

}
#endif

bool UI::FramelessWindow::nativeEvent(const QByteArray& eventType, void* message, qintptr* result) {
#ifdef Q_OS_WIN
    MSG* msg = static_cast<MSG*>(message);
    if (msg) {
        switch (msg->message) {
            case WM_NCACTIVATE:  // caption не рисуется при потере фокуса
                *result = TRUE;
                return true;
            case WM_NCPAINT:  // не даём DWM перерисовывать рамку
                *result = 0;
                return true;
            case WM_NCHITTEST: {
                RECT wr; GetWindowRect(msg->hwnd, &wr);
                const LONG x = GET_X_LPARAM(msg->lParam);
                const LONG y = GET_Y_LPARAM(msg->lParam);
                const LONG b = 8;
                if (!isMaximized()) {
                    const bool L = x < wr.left + b, R = x > wr.right - b;
                    const bool T = y < wr.top + b,  B = y > wr.bottom - b;
                    if (T && L) { *result = HTTOPLEFT;     return true; }
                    if (T && R) { *result = HTTOPRIGHT;    return true; }
                    if (B && L) { *result = HTBOTTOMLEFT;  return true; }
                    if (B && R) { *result = HTBOTTOMRIGHT; return true; }
                    if (L)      { *result = HTLEFT;        return true; }
                    if (R)      { *result = HTRIGHT;       return true; }
                    if (T)      { *result = HTTOP;         return true; }
                    if (B)      { *result = HTBOTTOM;      return true; }
                }

                POINT pt{ GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
                ScreenToClient(msg->hwnd, &pt);
                const qreal dpr = windowHandle()->devicePixelRatio();
                const QPoint posInWindow(qRound(pt.x / dpr), qRound(pt.y / dpr));

                if (auto* tb = findChild<UI::TopBar*>()) {
                    const auto hit = tb->hitTestTopBar(posInWindow);

                    if (hit == UI::TopBar::TopBarHit::Maximize) {
                        LRESULT dwmResult = 0;
                        DwmDefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam, &dwmResult);
                        if (dwmResult != 0) {
                            *result = dwmResult;
                            return true;
                        }
                        *result = HTMAXBUTTON;
                        return true;
                    }
                    if (hit == UI::TopBar::TopBarHit::Client) {
                        *result = HTCLIENT;
                        return true;
                    }
                    if (hit == UI::TopBar::TopBarHit::Caption) {
                        *result = HTCAPTION;
                        return true;
                    }
                }
                break;
            }

            case WM_NCMOUSEMOVE: {
                if (auto* tb = findChild<UI::TopBar*>()) {
                    tb->setMaximizeButtonHovered(false);
                    if (msg->wParam == HTMAXBUTTON) {
                        tb->setMaximizeButtonHovered(true);
                    }
                }
                if (msg->wParam == HTMAXBUTTON) {
                    TRACKMOUSEEVENT tme{ sizeof(tme) };
                    tme.dwFlags = TME_LEAVE | TME_NONCLIENT;
                    tme.hwndTrack = msg->hwnd;
                    TrackMouseEvent(&tme);
                }
                break;
            }

            case WM_NCMOUSELEAVE: {
                if (auto* tb = findChild<UI::TopBar*>()) {
                    tb->setMaximizeButtonHovered(false);
                }
                break;
            }

            case WM_NCLBUTTONDOWN: {
                if (msg->wParam == HTMAXBUTTON) {
                    *result = 0;
                    return true;
                }
                break;
            }

            case WM_NCLBUTTONUP: {
                if (msg->wParam == HTMAXBUTTON) {
                    if (auto* tb = findChild<UI::TopBar*>()) {
                        if (auto* maxBtn = tb->maximizeButton()) {
                            maxBtn->click();
                        }
                    }
                    *result = 0;
                    return true;
                }
                break;
            }

            case WM_MOUSEMOVE: {
                if (auto* tb = findChild<UI::TopBar*>()) {
                    tb->setMaximizeButtonHovered(false);
                }
                break;
            }
        }
    }
#endif
    return QMainWindow::nativeEvent(eventType, message, result);
}

void UI::FramelessWindow::changeEvent(QEvent* event) {
#ifdef Q_OS_WIN
    if (event->type() == QEvent::LanguageChange) {
        setWindowTitle(tr("OurPaint"));
        update();
    }
    if (auto* cw = centralWidget()) {
        cw->setGeometry(0, 0, width(), height());
        update();
    }
    if (event->type() == QEvent::WindowStateChange) {
        if (auto* tb = findChild<UI::TopBar*>()) {
            tb->updateMaximizeIcon(isMaximized());
        }
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
    for (const auto& file : files) {
        style += loadStyle(file);
    }
    return style;
}

void UI::FramelessWindow::showEvent(QShowEvent* event) {
#ifdef Q_OS_WIN
    applyNativeWindowStyles();
    if (auto* cw = centralWidget()) {
        cw->setGeometry(0, 0, width(), height());
        update();
    }
    if (auto *tb = findChild<UI::TopBar*>())
        tb->updateMaximizeIcon(isMaximized());
#endif
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

    qApp->setFont(font);
}

void UI::FramelessWindow::initTranslations() {
    /// lupdate . -ts Lib/translations/app_ru.ts
    /// lrelease Lib/translations/app_ru.ts

    static QTranslator translator;
    const bool ok = translator.load(":/translations/app_eng.qm");
    qApp->installTranslator(&translator);

    if (!ok) {
        qWarning() << "No translate files";
    }
}

void UI::FramelessWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
#ifdef Q_OS_WIN
    if (centralWidget()) {
        centralWidget()->setGeometry(0, 0, width(), height());
        update();
    }
#endif
}

bool UI::FramelessWindow::event(QEvent* event) {
    const bool result = QMainWindow::event(event);
#ifdef Q_OS_WIN
    if (event->type() == QEvent::LayoutRequest) {
        setContentsMargins(0, 0, 0, 0);

        if (auto* cw = centralWidget()) {
            cw->setContentsMargins(0, 0, 0, 0);
            if (auto* lay = cw->layout()) {
                lay->setContentsMargins(0, 0, 0, 0);
            }
            cw->setGeometry(0, 0, width(), height());
        }
    }
#endif
    return result;
}