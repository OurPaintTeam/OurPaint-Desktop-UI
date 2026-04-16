#ifndef OURPAINT_DESKTOP_UI_COMMANDCONSOLE_H
#define OURPAINT_DESKTOP_UI_COMMANDCONSOLE_H


#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

namespace UI {
    class CommandConsole final : public QWidget {
        Q_OBJECT

    public:
        explicit CommandConsole(QWidget* parent = nullptr);
        void setLineEditEngine(QLineEdit* lineEdit);

    signals:
        void sentCommandTriggered(const QString& command);

    private:
        QHBoxLayout* layout_{nullptr};
        QLineEdit* lineEdit_{nullptr};
        QPushButton* button_{nullptr};
    };
} // namespace UI


#endif //OURPAINT_DESKTOP_UI_COMMANDCONSOLE_H
