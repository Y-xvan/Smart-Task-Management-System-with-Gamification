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

// ---- Qt includes for GUI/QML mode (guarded) ----
#ifdef USE_QT_GUI
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFile>
#include <QDir>
#include <QDebug>
#endif

// 项目内部头（保持原有结构）
#include "database/DatabaseManager.h"
#include "ui/UIManager.h"
#include "Pomodoro/pomodoro.h"
#include "gamification/XPSystem.h"
#include "web/WebServer.h"

using namespace std;

// 控制台初始化占位（避免缺失头文件阻塞构建）
void setupConsole() {}

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

        // --- 决策：启动 本地 Web UI 还是 Console UI ---
        bool forceConsole = false;
        for (int i = 1; i < argc; ++i) {
            string a(argv[i]);
            if (a == "--console") forceConsole = true;
        }

        bool hasStdinTTY = ISATTY(FILENO(stdin));
        bool preferWeb = !forceConsole && !hasStdinTTY ? true : !forceConsole;

#ifdef USE_QT_GUI
        bool forceGui = false;
        for (int i = 1; i < argc; ++i) {
            string a(argv[i]);
            if (a == "--gui") forceGui = true;
        }
        if (forceGui) preferWeb = false;
#endif

        if (preferWeb) {
            // ---- 本地 Web UI ----
            cout << "\n\033[1;36m>> Launching local Web UI on http://127.0.0.1:8787 ...\033[0m\n";

            // 初始化各管理器
            auto reminderDAO = createReminderDAO("task_manager.db");
            TaskManager taskMgr;
            taskMgr.initialize();
            ProjectManager projMgr;
            projMgr.initialize();
            ReminderSystem reminderSys(std::move(reminderDAO));
            XPSystem xpSys;
            StatisticsAnalyzer statsAnalyzer;
            auto achievementDAO = std::make_unique<AchievementDAO>("./data/");
            AchievementManager achieveMgr(std::move(achievementDAO), 1);
            HeatmapVisualizer heatmap("task_manager.db");
            heatmap.initialize();
            Pomodoro pomodoro;
            achieveMgr.initialize();

            WebServer server(8787, "resources/web", &taskMgr, &projMgr, &reminderSys, &xpSys, &statsAnalyzer, &achieveMgr, &pomodoro, &heatmap);
            server.start();

            // 尝试自动打开默认浏览器
#ifdef _WIN32
            system("start http://127.0.0.1:8787");
#elif __APPLE__
            system("open http://127.0.0.1:8787");
#else
            system("xdg-open http://127.0.0.1:8787 >/dev/null 2>&1");
#endif

            cout << "\nPress Ctrl+C to exit the server.\n";
            while (true) {
                this_thread::sleep_for(chrono::seconds(1));
            }
        } else {
            // ---- 控制台 UI ----
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
