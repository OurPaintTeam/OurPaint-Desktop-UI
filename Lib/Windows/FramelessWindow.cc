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


UI::FramelessWindow::FramelessWindow(QWidget *parent) : QMainWindow(parent) {
    // setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
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
    initWindowForWindowsFrame();
    // initWindowForWindowsWithoutFrame();
#endif

    constexpr int sizeW = 960;
    constexpr int sizeY = 540;
    constexpr auto size = QSize(sizeW, sizeY);
    resize(size);
}

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


void UI::FramelessWindow::initFonts() {
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    // Загружаем ВСЕ веса
    int id1 = QFontDatabase::addApplicationFont(":/Styles/font/static/Inter-Regular.ttf");
    int id2 = QFontDatabase::addApplicationFont(":/Styles/font/static/Inter-Medium.ttf");
    int id3 = QFontDatabase::addApplicationFont(":/Styles/font/static/Inter-SemiBold.ttf");

    if (id1 == -1 || id2 == -1 || id3 == -1) {
        qDebug() << "Failed to load one of the fonts!";
    }

    // Берём family из ЛЮБОГО валидного id (например id1)
    QString family = QFontDatabase::applicationFontFamilies(id1).value(0);

    if (family.isEmpty()) {
        qDebug() << "Font family is empty!";
        return;
    }

    // Создаём шрифт
    QFont font(family);

    // Сглаживание
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


void UI::FramelessWindow::initWindowForWindowsFrame() const {
    HWND hwnd = reinterpret_cast<HWND>(winId());

    const COLORREF captionColor = RGB(31, 52, 56);

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

    const COLORREF borderColor = RGB(73, 72, 80);

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
                                      void *message, qintptr *result) {
    /*
     * Если initWindowForWindowsWithoutFrame()
     */
    /*
  #ifdef Q_OS_WIN
      if (eventType == "windows_generic_MSG") {
          MSG* msg = static_cast<MSG*>(message);

          switch (msg->message) {
              case WM_NCHITTEST: {
                  const LONG borderWidth = 8;

                  RECT winrect;
                  GetWindowRect(msg->hwnd, &winrect);

                  long x = GET_X_LPARAM(msg->lParam);
                  long y = GET_Y_LPARAM(msg->lParam);

                  bool left = x < winrect.left + borderWidth;
                  bool right = x > winrect.right - borderWidth;
                  bool top = y < winrect.top + borderWidth;
                  bool bottom = y > winrect.bottom - borderWidth;

                  if (left && top) {
                      *result = HTTOPLEFT;
                      return true;
                  }
                  if (right && top) {
                      *result = HTTOPRIGHT;
                      return true;
                  }
                  if (left && bottom) {
                      *result = HTBOTTOMLEFT;
                      return true;
                  }
                  if (right && bottom) {
                      *result = HTBOTTOMRIGHT;
                      return true;
                  }
                  if (left) {
                      *result = HTLEFT;
                      return true;
                  }
                  if (right) {
                      *result = HTRIGHT;
                      return true;
                  }
                  if (top) {
                      *result = HTTOP;
                      return true;
                  }
                  if (bottom) {
                      *result = HTBOTTOM;
                      return true;
                  }

                  *result = HTCLIENT;
                  return true;
              }
              default:;
          }
      }
  #endif
  */
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
void UI::FramelessWindow::initTranslations() {
    // TODO: Implement translations
}

void UI::FramelessWindow::initFonts() {
    // TODO: Implement fonts
}
