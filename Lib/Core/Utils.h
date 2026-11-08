#ifndef OURPAINT_UTILS_H
#define OURPAINT_UTILS_H
#include <QString>

namespace UI{
    struct FileData {
        qint64 id = -1;
        QString name;
    };

    struct ProjectData  {
        qint64 id = -1;
        QString name;
        QString path;
    };
}
#endif //OURPAINT_UTILS_H
