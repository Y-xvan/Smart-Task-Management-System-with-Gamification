#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <ctime>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#include <io.h>
#define ISATTY _isatty
#define FILENO _fileno
#else
#include <unistd.h>
#define ISATTY isatty
#define FILENO fileno
#endif

// ---- Qt includes for GUI/QML mode ----
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFile>
#include <QDir>
#include <QDebug>

// 项目内部头（保持原有结构）
#include "database/DatabaseManager.h"
#include "ui/UIManager.h"
#include "utils/platform.h" // 如果需要（按你仓库原来组织）
#include "Pomodoro/pomodoro.h"
#include "gamification/XPSystem.h"

using namespace std;

// 你原来的辅助函数（保留）
void sleepMs(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// 打字机效果：逐字输出
void typewriterPrint(const string& text, int speedMs = 20, string color = "\033[1;37m") {
    cout << color;
    for (char c : text) {
        cout << c << flush;
        sleepMs(speedMs);
    }
    cout << "\033[0m" << endl; // 重置颜色
}

// 模拟加载进度条
void simulateLoading(const string& taskName) {
    cout << "  " << taskName << " [";
    for (int i = 0; i < 20; ++i) {
        cout << "\033[1;36m#\033[0m" << flush; // 青色进度块
        sleepMs(30 + (rand() % 50)); // 随机延迟，模拟真实感
    }
    cout << "] \033[1;32mOK\033[0m" << endl;
    sleepMs(200);
}

/**
 * @brief 显示欢迎横幅 (增强版)
 */
void displayWelcomeBanner() {
    sleepMs(500);
    // 清屏指令：兼容不同终端
    cout << "\033[2J\033[H";

    sleepMs(800);

    // 欢迎语
    typewriterPrint("    >> Welcome back, Hero.", 40, "\033[1;33m"); // 黄色
    typewriterPrint("    >> Preparing your workspace...", 30, "\033[0m");
    cout << endl;
}

/**
 * @brief 初始化系统 (游戏化文案)
 * @return 是否初始化成功
 */
bool initializeSystem() {
    // 1. 初始化数据库 -> "Opening Quest Log"
    simulateLoading("Opening Quest Log (DB)   ");

    DatabaseManager& db = DatabaseManager::getInstance();

    if (!db.initialize("task_manager.db")) {
        cerr << "\033[1;31m[CRITICAL ERROR] The Quest Log is corrupted!\033[0m" << endl;
        cerr << "Error: " << db.getLastErrorMessage() << endl;
        return false;
    }

    // 2. 验证数据库表 -> "Verifying World State"
    simulateLoading("Verifying World State    ");

    bool allTablesExist = true;
    vector<string> requiredTables = {
        "tasks", "projects", "challenges", "reminders",
        "achievements", "user_stats", "user_settings"
    };

    for (const string& table : requiredTables) {
        if (!db.tableExists(table)) {
            cerr << "\033[1;31m[MISSING] Artifact '" << table << "' not found.\033[0m" << endl;
            allTablesExist = false;
        }
    }

    if (!allTablesExist) {
        return false;
    }

    // 3. 检查完整性 -> "Syncing with Server"
    simulateLoading("Syncing Player Stats     ");

    if (!db.checkDatabaseIntegrity()) {
        cerr << "\033[1;31m[ERROR] Data integrity breach detected!\033[0m" << endl;
        return false;
    }

    cout << "\n";
    typewriterPrint(">> System ready. Let's get things done.", 20, "\033[1;32m");
    cout << "\n";

    // 给用户一点时间看清楚加载结果
    sleepMs(1000);

    return true;
}

/**
 * @brief 清理系统资源
 */
void cleanupSystem() {
    cout << "\n\033[1;33m>> Saving progress...\033[0m\n";
    sleepMs(500);

    // 关闭数据库连接
    DatabaseManager::destroyInstance();

    simulateLoading("Closing Quest Log        ");
}

/**
 * @brief 主函数
 */
int main(int argc, char* argv[]) {
    // === 步骤 0: 设置控制台环境 (修复乱码的关键) ===
    setupConsole();

    // 初始化随机数种子 (让加载动画的延迟真正随机)
    srand(static_cast<unsigned int>(time(nullptr)));

    try {
        // 1. 酷炫的开场
        displayWelcomeBanner();

        // 2. 游戏化的初始化流程
        if (!initializeSystem()) {
            cerr << "\n\033[1;31m💥 System Launch Aborted.\033[0m\n";
            // 防止窗口在报错后瞬间关闭
            cout << "Press ENTER to exit...";
            cin.get();
            return 1;
        }

        // --- 决策：启动 GUI(QML) 还是 Console UI ---
        bool forceGui = false;
        bool forceConsole = false;
        for (int i = 1; i < argc; ++i) {
            string a(argv[i]);
            if (a == "--gui") forceGui = true;
            if (a == "--console") forceConsole = true;
        }

        bool hasStdinTTY = ISATTY(FILENO(stdin));
        bool preferGui = forceGui || (!hasStdinTTY && !forceConsole);

        if (preferGui) {
            // ---- GUI 启动分支 (QML) ----
            // 使用 QApplication，因为 QML Controls 需要 GUI 支持
            QApplication app(argc, argv);

            QQmlApplicationEngine engine;

            // 优先尝试 exe 旁的 resources/qml/AppLauncher.qml（便于开发）
            QString localQml = QCoreApplication::applicationDirPath() + "/resources/qml/AppLauncher.qml";
            QUrl url;
            if (QFile::exists(localQml)) {
                url = QUrl::fromLocalFile(localQml);
            }
            else {
                // 如果你把 QML 放进 qrc，使用 qrc 路径 (qrc prefix 在 qml.qrc 中我们使用 /qml)
                url = QUrl(QStringLiteral("qrc:/qml/AppLauncher.qml"));
            }

            qDebug() << "Loading QML from" << url.toString();
            engine.load(url);

            if (engine.rootObjects().isEmpty()) {
                qCritical() << "Failed to load QML root object:" << url;
                // GUI 启动失败，进行清理并退出（回退到控制台模式不是自动尝试的策略）
                cleanupSystem();
                return -1;
            }

            // 在 GUI 程序退出后继续清理
            int ret = app.exec();
            cleanupSystem();
            return ret;
        }
        else {
            // ---- 保持原来的控制台 UI 启动 ----
            cout << "\033[1;36m>> Press ENTER to Start Session <<\033[0m";
            cin.get();

            UIManager* ui = new UIManager();
            ui->run();

            // 4. 优雅退出
            delete ui;
            cleanupSystem();

            cout << "\n";
            cout << "\033[1;36m"; // 青色
            cout << "╔═══════════════════════════════════════════════════╗\n";
            cout << "║                                                   ║\n";
            cout << "║      See you next time. Keep the streak! 🔥       ║\n";
            cout << "║                                                   ║\n";
            cout << "╚═══════════════════════════════════════════════════╝\n";
            cout << "\033[0m\n";

            return 0;
        }

    }
    catch (const exception& e) {
        cerr << "\n\033[1;31m[CRITICAL EXCEPTION] " << e.what() << "\033[0m\n";
        cleanupSystem();
        return 1;
    }
    catch (...) {
        cerr << "\n\033[1;31m[UNKNOWN ERROR] The system crashed unexpectedly.\033[0m\n";
        cleanupSystem();
        return 1;
    }
}