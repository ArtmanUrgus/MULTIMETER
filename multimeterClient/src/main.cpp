#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "gui/MultimeterClientManager.h"

#include <QString>
#include <QTimer>

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    MultimeterClientManager mulimeter{&app};
    mulimeter.init();

    return app.exec();
}
