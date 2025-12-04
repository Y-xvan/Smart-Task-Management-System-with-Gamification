#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "ui/TaskModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    // Set application metadata
    app.setApplicationName("Smart Task Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("SmartTaskGUI");
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Create and register TaskModel as context property
    TaskModel taskModel;
    engine.rootContext()->setContextProperty("taskModel", &taskModel);
    
    // Also register TaskModel as a QML type for potential instantiation
    qmlRegisterType<TaskModel>("SmartTaskGUI", 1, 0, "TaskModel");
    
    // Load the main QML file
    const QUrl url(QStringLiteral("qrc:/SmartTaskGUI/resources/qml/Main.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    return app.exec();
}
