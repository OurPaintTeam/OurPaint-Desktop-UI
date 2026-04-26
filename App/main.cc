#include <QApplication>

#include "UIManager.h"

#include <QTranslator>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(resources);
    Q_INIT_RESOURCE(translations);

  QTranslator translator;
  translator.load(":/translations/app_ru.qm");
  app.installTranslator(&translator);

    new UIManager();
    return QApplication::exec();
}
