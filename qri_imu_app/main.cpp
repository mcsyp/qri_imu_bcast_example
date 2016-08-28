#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "singlescanner.h"
#include "myqmlcontroller.h"
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    MyQmlController controller;

    QQmlApplicationEngine engine;
    QQmlContext * ctx = engine.rootContext();
    ctx->setContextProperty("cstController",&controller);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
