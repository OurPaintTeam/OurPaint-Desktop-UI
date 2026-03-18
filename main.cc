#include <QApplication>

#include "UIManager.h"


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    new UI::UIManager();
    return QApplication::exec();
}
