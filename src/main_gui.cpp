#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <clocale>

#include "ui/TaskModel.h"
#include "ui/ProjectModel.h"
#include "ui/ReminderModel.h"
#include "ui/GameController.h"

#include "task/TaskManager.h"
#include "project/ProjectManager.h"
#include "gamification/XPSystem.h"
#include "statistics/StatisticsAnalyzer.h"
#include "Pomodoro/pomodoro.h"
#include "reminder/ReminderSystem.h"
#include "database/DAO/ReminderDAO.h"
#include "achievement/AchievementManager.h"
#include "database/DAO/AchievementDAO.h"

int main(int argc, char* argv[]) {
    // ǿ�� UTF-8 ����
    std::setlocale(LC_ALL, "en_US.UTF-8");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    app.setOrganizationName("HeroGuild");
    app.setApplicationName("SmartTaskRPG");

    // === 1. ��ʼ����˺���ҵ�� ===
    // ʹ�� static ��֤�������ڸ������� App ������
    static TaskManager taskMgr;
    static ProjectManager projectMgr;
    static XPSystem xpSys;
    static StatisticsAnalyzer statsAna;
    static Pomodoro pomodoro;

    // ��ʼ������ϵͳ
    auto reminderDAO = createReminderDAO("task_manager.db");
    static ReminderSystem reminderSys(std::move(reminderDAO));

    // ��ʼ���ɾ�ϵͳ
    auto achieveDAO = std::make_unique<AchievementDAO>("./data/");
    static AchievementManager achieveMgr(std::move(achieveDAO), 1);

    // ���ݿ��ʼ�����
    if (!taskMgr.initialize()) {
        qWarning("CRITICAL: Database initialization failed!");
    }

    // === 2. ���� ViewModel ������ ===
    TaskModel taskModel(&taskMgr, &projectMgr, &xpSys);
    ProjectModel projectModel(&projectMgr);
    ReminderModel reminderModel(&reminderSys);
    GameController gameController(&xpSys, &statsAna, &pomodoro, &achieveMgr);

    // === 3. ���� QML ���� ===
    QQmlApplicationEngine engine;

    // ע�� C++ ���� QML
    engine.rootContext()->setContextProperty("taskModel", &taskModel);
    engine.rootContext()->setContextProperty("projectModel", &projectModel);
    engine.rootContext()->setContextProperty("reminderModel", &reminderModel);
    engine.rootContext()->setContextProperty("gameController", &gameController);

    const QUrl url(QStringLiteral("qrc:/qml/MainView.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}