#ifndef OURPAINT_REGWIN_H
#define OURPAINT_REGWIN_H

#include <QSettings>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
namespace AppUI {
    void registerFileAssociation() {
        const QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());

        QSettings registry("HKEY_CURRENT_USER\\Software\\Classes", QSettings::NativeFormat);

        registry.setValue(".ourp/.", "OurPaint.Project");

        registry.setValue("OurPaint.Project/.", "OurPaint Project File");

        registry.setValue("OurPaint.Project/DefaultIcon/.", appPath + ",0");


        registry.setValue("OurPaint.Project/shell/open/command/.",
                          QStringLiteral("\"%1\" \"%2\"").arg(appPath, "%1"));

        registry.sync();
    }
}
#endif //OURPAINT_REGWIN_H
