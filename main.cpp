#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "appcore.h"


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    AppCore appCore;

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);




    qmlRegisterType<AppCore>("AppCoreClass", 1, 0, "AppCore");
    // engine.rootContext()->setContextProperty("activeBoot", appCore.getActiveBoot());
    engine.rootContext()->setContextProperty("appCore", &appCore);
    engine.load(url);

    appCore.checkRecoveryMod();

    for(int i=0; i<argc; i++){
        if(i == 1){
            qDebug()<<"Path:"<<argv[i];
            appCore.setFilePatch(argv[i]);
        }
    }


    return app.exec();
}
