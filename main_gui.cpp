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
    // 强制 UTF-8 环境
    std::setlocale(LC_ALL, "en_US.UTF-8");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    app.setOrganizationName("HeroGuild");
    app.setApplicationName("SmartTaskRPG");

    // === 1. 初始化后端核心业务 ===
    // 使用 static 保证生命周期覆盖整个 App 运行期
    static TaskManager taskMgr;
    static ProjectManager projectMgr;
    static XPSystem xpSys;
    static StatisticsAnalyzer statsAna;
    static Pomodoro pomodoro;

    // 初始化提醒系统
    auto reminderDAO = createReminderDAO("task_manager.db");
    static ReminderSystem reminderSys(std::move(reminderDAO));

    // 初始化成就系统
    auto achieveDAO = std::make_unique<AchievementDAO>("./data/");
    static AchievementManager achieveMgr(std::move(achieveDAO), 1);

    // 数据库初始化检查
    if (!taskMgr.initialize()) {
        qWarning("CRITICAL: Database initialization failed!");
    }

    // === 2. 创建 ViewModel 适配器 ===
    TaskModel taskModel(&taskMgr, &projectMgr, &xpSys);
    ProjectModel projectModel(&projectMgr);
    ReminderModel reminderModel(&reminderSys);
    GameController gameController(&xpSys, &statsAna, &pomodoro, &achieveMgr);

    // === 3. 启动 QML 引擎 ===
    QQmlApplicationEngine engine;

    // 注入 C++ 对象到 QML
    engine.rootContext()->setContextProperty("taskModel", &taskModel);
    engine.rootContext()->setContextProperty("projectModel", &projectModel);
    engine.rootContext()->setContextProperty("reminderModel", &reminderModel);
    engine.rootContext()->setContextProperty("gameController", &gameController);

    const QUrl url(u"qrc:/resources/qml/MainView.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}