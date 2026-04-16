#include <QApplication>

#include "UIManager.h"


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(resources);
    new UIManager();
    return QApplication::exec();
}
