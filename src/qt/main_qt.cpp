/**
 * @file main_qt.cpp
 * @brief Qt/QML Application Entry Point
 * 
 * Smart Task Management System with Gamification
 * Duolingo-style Interactive UI
 * 
 * This is the entry point for the Qt/QML GUI application.
 * It bridges the existing C++ business logic with the QML UI.
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QIcon>

#include "models/TaskListModel.h"
#include "models/ProjectListModel.h"
#include "models/ChallengeListModel.h"
#include "controllers/TaskController.h"
#include "controllers/GamificationController.h"

#include "database/DatabaseManager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    // Application metadata
    app.setApplicationName("Smart Task Manager");
    app.setOrganizationName("TaskManagerTeam");
    app.setApplicationVersion("1.0.0");
    
    // Set Material Design style for Duolingo-like appearance
    QQuickStyle::setStyle("Material");
    
    // Initialize database
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.initialize("task_manager.db")) {
        qCritical() << "Failed to initialize database!";
        return -1;
    }
    
    // Create controllers and models
    TaskController taskController;
    GamificationController gamificationController;
    
    TaskListModel taskListModel;
    ProjectListModel projectListModel;
    ChallengeListModel challengeListModel;
    
    // Connect models to controllers
    taskListModel.setController(&taskController);
    
    // Create QML engine
    QQmlApplicationEngine engine;
    
    // Expose C++ objects to QML
    QQmlContext* context = engine.rootContext();
    context->setContextProperty("taskController", &taskController);
    context->setContextProperty("gamificationController", &gamificationController);
    context->setContextProperty("taskListModel", &taskListModel);
    context->setContextProperty("projectListModel", &projectListModel);
    context->setContextProperty("challengeListModel", &challengeListModel);
    
    // Load main QML file
    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    int result = app.exec();
    
    // Cleanup
    DatabaseManager::destroyInstance();
    
    return result;
}
