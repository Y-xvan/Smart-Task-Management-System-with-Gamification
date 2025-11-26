#include "ui/UIManager.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <ctime>

using namespace std;

// ANSI颜色定义
const string UIManager::COLOR_RESET = "\033[0m";
const string UIManager::COLOR_RED = "\033[31m";
const string UIManager::COLOR_GREEN = "\033[32m";
const string UIManager::COLOR_YELLOW = "\033[33m";
const string UIManager::COLOR_BLUE = "\033[34m";
const string UIManager::COLOR_MAGENTA = "\033[35m";
const string UIManager::COLOR_CYAN = "\033[36m";
const string UIManager::COLOR_WHITE = "\033[37m";
const string UIManager::BOLD = "\033[1m";
const string UIManager::UNDERLINE = "\033[4m";

UIManager::UIManager() {
    running = true;
    
    // 初始化随机数种子
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // 创建各模块实例
    statsAnalyzer = new StatisticsAnalyzer();
    xpSystem = new XPSystem();
    heatmap = new HeatmapVisualizer();
    projectManager = new ProjectManager();
    taskManager = new TaskManager();
    taskManager->initialize();
    
    cout << "✅ UI管理器初始化成功" << endl;
}

UIManager::~UIManager() {
    delete statsAnalyzer;
    delete xpSystem;
    delete heatmap;
    delete projectManager;
    delete taskManager;
}

// === UI辅助方法 ===

void UIManager::clearScreen() {
    // ANSI转义序列清屏
    cout << "\033[2J\033[H";
}

void UIManager::printHeader(const string& title) {
    cout << "\n";
    cout << BOLD << COLOR_CYAN;
    printSeparator('=', 55);
    cout << "    " << title << "\n";
    printSeparator('=', 55);
    cout << COLOR_RESET << "\n";
}

void UIManager::printSeparator(char symbol, int length) {
    for (int i = 0; i < length; i++) {
        cout << symbol;
    }
    cout << "\n";
}

void UIManager::printMenu(const vector<string>& options) {
    cout << "\n";
    for (size_t i = 0; i < options.size(); i++) {
        cout << "  " << COLOR_YELLOW << i + 1 << COLOR_RESET 
             << ". " << options[i] << "\n";
    }
    cout << "  " << COLOR_RED << "0" << COLOR_RESET << ". 返回/退出\n";
    cout << "\n";
}

int UIManager::getUserChoice(int maxChoice) {
    int choice;
    cout << COLOR_GREEN << "请选择 (0-" << maxChoice << "): " << COLOR_RESET;
    
    while (!(cin >> choice) || choice < 0 || choice > maxChoice) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << COLOR_RED << "无效输入！请重新选择 (0-" << maxChoice << "): " << COLOR_RESET;
    }
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return choice;
}

string UIManager::getInput(const string& prompt) {
    string input;
    cout << COLOR_GREEN << prompt << COLOR_RESET;
    getline(cin, input);
    return input;
}

int UIManager::getIntInput(const string& prompt) {
    int value;
    cout << COLOR_GREEN << prompt << COLOR_RESET;
    
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << COLOR_RED << "无效输入！请输入数字: " << COLOR_RESET;
    }
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

void UIManager::pause() {
    cout << "\n" << COLOR_YELLOW << "按Enter键继续..." << COLOR_RESET;
    cin.get();
}

bool UIManager::confirmAction(const string& prompt) {
    cout << COLOR_YELLOW << prompt << " (y/n): " << COLOR_RESET;
    string response;
    getline(cin, response);
    return (response == "y" || response == "Y" || response == "yes" || response == "YES");
}

void UIManager::displayUserStatusBar() {
    int level = xpSystem->getCurrentLevel();
    int totalXP = xpSystem->getTotalXP();
    int nextLevelXP = xpSystem->getXPForNextLevel();
    string title = xpSystem->getCurrentLevelTitle();
    int achievements = statsAnalyzer->getAchievementsUnlocked();
    
    cout << BOLD << COLOR_CYAN;
    printSeparator('-', 55);
    cout << COLOR_RESET;
    
    cout << COLOR_MAGENTA << "👤 " << COLOR_RESET 
         << "等级 " << BOLD << level << COLOR_RESET 
         << " (" << title << ") | "
         << COLOR_YELLOW << "XP: " << COLOR_RESET << totalXP << "/" << nextLevelXP << " | "
         << COLOR_GREEN << "⭐ 成就: " << COLOR_RESET << achievements << "\n";
    
    cout << BOLD << COLOR_CYAN;
    printSeparator('-', 55);
    cout << COLOR_RESET << "\n";
}

void UIManager::displayHUD() {
    int level = xpSystem->getCurrentLevel();
    int totalXP = xpSystem->getTotalXP();
    int nextLevelXP = xpSystem->getXPForNextLevel();
    string title = xpSystem->getCurrentLevelTitle();
    double progress = xpSystem->getLevelProgress();
    int achievements = statsAnalyzer->getAchievementsUnlocked();
    string badge = xpSystem->getLevelBadge(level);
    
    // 顶部装饰线
    cout << BOLD << COLOR_CYAN;
    printSeparator('=', 55);
    cout << COLOR_RESET;
    
    // 用户信息行
    cout << " " << badge << " " << BOLD << COLOR_MAGENTA << "Lv." << level << COLOR_RESET 
         << " " << COLOR_YELLOW << title << COLOR_RESET << "  |  "
         << COLOR_GREEN << "⭐ " << achievements << " 成就" << COLOR_RESET << "\n";
    
    // XP 进度条
    cout << "    ";
    printProgressBar(progress, 30);
    cout << " " << COLOR_CYAN << totalXP << "/" << nextLevelXP << " XP" << COLOR_RESET << "\n";
    
    // 励志名言
    printEncouragement();
    
    // 底部装饰线
    cout << BOLD << COLOR_CYAN;
    printSeparator('=', 55);
    cout << COLOR_RESET << "\n";
}

void UIManager::printProgressBar(double progress, int width) {
    int filled = static_cast<int>(progress * width);
    
    cout << COLOR_GREEN << "[";
    for (int i = 0; i < width; i++) {
        if (i < filled) {
            cout << "#";
        } else {
            cout << "-";
        }
    }
    cout << "]" << COLOR_RESET;
}

void UIManager::printEncouragement() {
    vector<string> quotes = {
        "Keep going! You're doing great! 💪",
        "Every task completed is a step forward! 🚀",
        "Stay focused and conquer your goals! 🎯",
        "You're on fire! Keep up the momentum! 🔥",
        "Success is built one task at a time! ⭐",
        "Believe in yourself, you can do it! 💫",
        "Progress, not perfection! 🌟",
        "Your dedication is inspiring! 🏆"
    };
    
    int index = rand() % quotes.size();
    cout << "    " << COLOR_YELLOW << quotes[index] << COLOR_RESET << "\n";
}

void UIManager::showTaskCompleteCelebration(int xpEarned) {
    cout << "\n";
    cout << COLOR_GREEN << "  ╔═══════════════════════════════════════╗\n";
    cout << "  ║      🎉 TASK COMPLETED! 🎉            ║\n";
    cout << "  ╠═══════════════════════════════════════╣\n";
    cout << "  ║                                       ║\n";
    cout << "  ║      +" << setw(4) << xpEarned << " XP EARNED!               ║\n";
    cout << "  ║                                       ║\n";
    cout << "  ║    ⭐ ⭐ ⭐ EXCELLENT! ⭐ ⭐ ⭐        ║\n";
    cout << "  ║                                       ║\n";
    cout << "  ╚═══════════════════════════════════════╝" << COLOR_RESET << "\n\n";
}

// === 主界面 ===

void UIManager::showMainMenu() {
    clearScreen();
    
    cout << BOLD << COLOR_BLUE;
    cout << R"(
   ╔═══════════════════════════════════════════════════╗
   ║    🎮 Smart Task Management System v1.0 🎮        ║
   ╚═══════════════════════════════════════════════════╝
)" << COLOR_RESET;
    
    displayHUD();
    
    vector<string> options = {
        "📋 任务管理",
        "📁 项目管理",
        "📊 统计分析",
        "🎮 游戏化功能",
        "⚙️  设置"
    };
    
    printMenu(options);
}

void UIManager::run() {
    cout << COLOR_GREEN << "\n🎉 欢迎使用智能任务管理系统！\n" << COLOR_RESET;
    pause();
    
    while (running) {
        showMainMenu();
        int choice = getUserChoice(5);
        
        switch (choice) {
            case 1:
                showTaskMenu();
                break;
            case 2:
                showProjectMenu();
                break;
            case 3:
                showStatisticsMenu();
                break;
            case 4:
                showGamificationMenu();
                break;
            case 5:
                showSettingsMenu();
                break;
            case 0:
                exitProgram();
                break;
        }
    }
}

void UIManager::exitProgram() {
    if (confirmAction("确定要退出吗？")) {
        clearScreen();
        cout << COLOR_GREEN << "\n👋 感谢使用！再见！\n\n" << COLOR_RESET;
        running = false;
    }
}

// === 任务管理界面 ===

void UIManager::showTaskMenu() {
    clearScreen();
    printHeader("📋 任务管理");
    
    vector<string> options = {
        "创建新任务",
        "查看所有任务",
        "更新任务",
        "删除任务",
        "完成任务"
    };
    
    printMenu(options);
    int choice = getUserChoice(5);
    
    switch (choice) {
        case 1: createTask(); break;
        case 2: listTasks(); break;
        case 3: updateTask(); break;
        case 4: deleteTask(); break;
        case 5: completeTask(); break;
        case 0: return;
    }
}

void UIManager::createTask() {
    clearScreen();
    printHeader("✨ 创建新任务");
    
    string name = getInput("任务名称: ");
    if (name.empty()) {
        displayError("任务名称不能为空！");
        pause();
        return;
    }
    
    string desc = getInput("任务描述: ");
    int projectId = getIntInput("关联项目ID (0表示无): ");
    
    Task task(name, desc, projectId);
    int id = taskManager->createTask(task);
    
    if (id > 0) {
        displaySuccess("任务创建成功！ID: " + to_string(id));
    } else {
        displayError("任务创建失败！");
    }
    
    pause();
}

void UIManager::listTasks() {
    clearScreen();
    printHeader("📋 任务列表");
    
    vector<Task> tasks = taskManager->getAllTasks();
    
    if (tasks.empty()) {
        displayInfo("暂无任务");
    } else {
        cout << "\n";
        printSeparator('-', 55);
        
        for (const Task& t : tasks) {
            string status = t.isCompleted() ? COLOR_GREEN + "[完成]" + COLOR_RESET : COLOR_YELLOW + "[进行中]" + COLOR_RESET;
            cout << COLOR_BLUE << "ID: " << t.getId() << COLOR_RESET << " " << status << " "
                 << BOLD << t.getName() << COLOR_RESET << "\n";
            cout << "  描述: " << t.getDescription() << "\n";
            if (t.getProjectId() > 0) {
                cout << "  项目ID: " << t.getProjectId() << "\n";
            }
            printSeparator('-', 55);
        }
        
        // 显示统计信息
        int total = taskManager->getTaskCount();
        int completed = taskManager->getCompletedTaskCount();
        cout << "\n" << COLOR_CYAN << "总计: " << total << " 个任务, " << completed << " 个已完成" << COLOR_RESET << "\n";
    }
    
    pause();
}

void UIManager::updateTask() {
    clearScreen();
    printHeader("✏️  更新任务");
    
    int id = getIntInput("请输入任务ID: ");
    auto taskOpt = taskManager->getTask(id);
    
    if (!taskOpt.has_value()) {
        displayError("任务不存在！");
        pause();
        return;
    }
    
    Task task = taskOpt.value();
    cout << "\n当前任务: " << task.getName() << "\n";
    cout << "描述: " << task.getDescription() << "\n\n";
    
    string name = getInput("新名称 (留空保持不变): ");
    if (!name.empty()) {
        task.setName(name);
    }
    
    string desc = getInput("新描述 (留空保持不变): ");
    if (!desc.empty()) {
        task.setDescription(desc);
    }
    
    if (taskManager->updateTask(task)) {
        displaySuccess("任务更新成功！");
    } else {
        displayError("任务更新失败！");
    }
    
    pause();
}

void UIManager::deleteTask() {
    clearScreen();
    printHeader("🗑️  删除任务");
    
    int id = getIntInput("请输入要删除的任务ID: ");
    
    auto taskOpt = taskManager->getTask(id);
    if (!taskOpt.has_value()) {
        displayError("任务不存在！");
        pause();
        return;
    }
    
    Task task = taskOpt.value();
    cout << "\n任务: " << task.getName() << "\n";
    
    if (confirmAction("确定要删除这个任务吗？")) {
        if (taskManager->deleteTask(id)) {
            displaySuccess("任务删除成功！");
        } else {
            displayError("删除失败！");
        }
    }
    
    pause();
}

void UIManager::completeTask() {
    clearScreen();
    printHeader("✅ 完成任务");
    
    // 先显示未完成的任务列表
    vector<Task> pendingTasks = taskManager->getTasksByCompletion(false);
    
    if (pendingTasks.empty()) {
        displayInfo("没有待完成的任务！");
        pause();
        return;
    }
    
    cout << "\n未完成的任务:\n";
    printSeparator('-', 55);
    for (const Task& t : pendingTasks) {
        cout << COLOR_BLUE << "ID: " << t.getId() << COLOR_RESET << " - "
             << BOLD << t.getName() << COLOR_RESET << "\n";
    }
    printSeparator('-', 55);
    
    int id = getIntInput("\n请输入要完成的任务ID: ");
    
    auto taskOpt = taskManager->getTask(id);
    if (!taskOpt.has_value()) {
        displayError("任务不存在！");
        pause();
        return;
    }
    
    Task task = taskOpt.value();
    if (task.isCompleted()) {
        displayWarning("该任务已经完成！");
        pause();
        return;
    }
    
    if (taskManager->completeTask(id)) {
        // 计算并奖励经验值
        int xpEarned = xpSystem->getXPForTaskCompletion(1); // 中优先级任务
        xpSystem->awardXP(xpEarned, "完成任务: " + task.getName());
        
        // 显示庆祝动画
        showTaskCompleteCelebration(xpEarned);
        
        displaySuccess("任务 \"" + task.getName() + "\" 已完成！");
    } else {
        displayError("完成任务失败！");
    }
    
    pause();
}

// === 项目管理界面 ===

void UIManager::showProjectMenu() {
    clearScreen();
    printHeader("📁 项目管理");
    
    vector<string> options = {
        "创建新项目",
        "查看所有项目",
        "查看项目详情",
        "更新项目",
        "删除项目"
    };
    
    printMenu(options);
    int choice = getUserChoice(5);
    
    switch (choice) {
        case 1: createProject(); break;
        case 2: listProjects(); break;
        case 3: viewProjectDetails(); break;
        case 4: updateProject(); break;
        case 5: deleteProject(); break;
        case 0: return;
    }
}

void UIManager::createProject() {
    clearScreen();
    printHeader("✨ 创建新项目");
    
    string name = getInput("项目名称: ");
    if (name.empty()) {
        displayError("项目名称不能为空！");
        pause();
        return;
    }
    
    string desc = getInput("项目描述: ");
    string color = getInput("颜色标签 (如 #4CAF50): ");
    
    Project project(name, desc, color);
    int id = projectManager->createProject(project);
    
    displaySuccess("项目创建成功！ID: " + to_string(id));
    
    pause();
}

void UIManager::listProjects() {
    clearScreen();
    printHeader("📁 项目列表");
    
    vector<Project*> projects = projectManager->getAllProjects();
    
    if (projects.empty()) {
        displayInfo("暂无项目");
    } else {
        cout << "\n";
        printSeparator('-', 55);
        
        for (Project* p : projects) {
            cout << COLOR_BLUE << "ID: " << p->getId() << COLOR_RESET << " | "
                 << BOLD << p->getName() << COLOR_RESET << "\n";
            cout << "  描述: " << p->getDescription() << "\n";
            cout << "  进度: " << COLOR_GREEN 
                 << fixed << setprecision(1) << (p->getProgress() * 100) << "%" 
                 << COLOR_RESET << " ("
                 << p->getCompletedTasks() << "/" << p->getTotalTasks() << ")\n";
            printSeparator('-', 55);
        }
    }
    
    pause();
}

void UIManager::viewProjectDetails() {
    clearScreen();
    printHeader("📊 项目详情");
    
    int id = getIntInput("请输入项目ID: ");
    Project* p = projectManager->getProject(id);
    
    if (p == nullptr) {
        displayError("项目不存在！");
    } else {
        cout << "\n";
        cout << BOLD << "项目名称: " << COLOR_RESET << p->getName() << "\n";
        cout << "描述: " << p->getDescription() << "\n";
        cout << "颜色标签: " << p->getColorLabel() << "\n";
        cout << "进度: " << COLOR_GREEN << fixed << setprecision(1) 
             << (p->getProgress() * 100) << "%" << COLOR_RESET << "\n";
        cout << "任务统计: " << p->getCompletedTasks() << "/" << p->getTotalTasks() << "\n";
        cout << "目标日期: " << p->getTargetDate() << "\n";
        cout << "创建日期: " << p->getCreatedDate() << "\n";
        cout << "状态: " << (p->isArchived() ? "已归档" : "活跃") << "\n";
    }
    
    pause();
}

void UIManager::updateProject() {
    clearScreen();
    printHeader("✏️  更新项目");
    
    displayInfo("功能开发中...");
    
    pause();
}

void UIManager::deleteProject() {
    clearScreen();
    printHeader("🗑️  删除项目");
    
    int id = getIntInput("请输入要删除的项目ID: ");
    
    if (confirmAction("确定要删除这个项目吗？")) {
        if (projectManager->deleteProject(id)) {
            displaySuccess("项目删除成功！");
        } else {
            displayError("删除失败！");
        }
    }
    
    pause();
}

// === 统计分析界面 ===

void UIManager::showStatisticsMenu() {
    clearScreen();
    printHeader("📊 统计分析");
    
    vector<string> options = {
        "统计数据总览",
        "每日报告",
        "每周报告",
        "每月报告",
        "任务完成热力图"
    };
    
    printMenu(options);
    int choice = getUserChoice(5);
    
    switch (choice) {
        case 1: showStatisticsSummary(); break;
        case 2: showDailyReport(); break;
        case 3: showWeeklyReport(); break;
        case 4: showMonthlyReport(); break;
        case 5: showHeatmap(); break;
        case 0: return;
    }
}

void UIManager::showStatisticsSummary() {
    clearScreen();
    printHeader("📈 统计数据总览");
    
    cout << statsAnalyzer->generateSummary();
    
    pause();
}

void UIManager::showDailyReport() {
    clearScreen();
    printHeader("📅 每日报告");
    
    cout << statsAnalyzer->generateDailyReport();
    
    pause();
}

void UIManager::showWeeklyReport() {
    clearScreen();
    printHeader("📈 每周报告");
    
    cout << statsAnalyzer->generateWeeklyReport();
    
    pause();
}

void UIManager::showMonthlyReport() {
    clearScreen();
    printHeader("📊 每月报告");
    
    cout << statsAnalyzer->generateMonthlyReport();
    
    pause();
}

void UIManager::showHeatmap() {
    clearScreen();
    printHeader("🔥 任务完成热力图");
    
    // 显示热力图（数据从数据库中获取）
    cout << heatmap->generateHeatmap(90);
    
    pause();
}

// === 游戏化界面 ===

void UIManager::showGamificationMenu() {
    clearScreen();
    printHeader("🎮 游戏化功能");
    
    vector<string> options = {
        "经验值和等级",
        "成就系统",
        "挑战系统"
    };
    
    printMenu(options);
    int choice = getUserChoice(3);
    
    switch (choice) {
        case 1: showXPAndLevel(); break;
        case 2: showAchievements(); break;
        case 3: showChallenges(); break;
        case 0: return;
    }
}

void UIManager::showXPAndLevel() {
    clearScreen();
    printHeader("⭐ 经验值和等级");
    
    cout << xpSystem->displayLevelInfo();
    
    pause();
}

void UIManager::showAchievements() {
    clearScreen();
    printHeader("🏆 成就系统");
    
    int unlocked = statsAnalyzer->getAchievementsUnlocked();
    
    cout << "\n已解锁成就: " << COLOR_GREEN << unlocked << COLOR_RESET << " 个\n\n";
    
    displayInfo("成就系统详细功能开发中...");
    displayInfo("需要成就模块完全实现后集成");
    
    pause();
}

void UIManager::showChallenges() {
    clearScreen();
    printHeader("🎯 挑战系统");
    
    int completed = statsAnalyzer->getChallengesCompleted();
    
    cout << "\n已完成挑战: " << COLOR_GREEN << completed << COLOR_RESET << " 个\n\n";
    
    displayInfo("挑战系统详细功能开发中...");
    displayInfo("需要挑战模块完全实现后集成");
    
    pause();
}

// === 设置界面 ===

void UIManager::showSettingsMenu() {
    clearScreen();
    printHeader("⚙️  系统设置");
    
    vector<string> options = {
        "查看设置",
        "修改设置"
    };
    
    printMenu(options);
    int choice = getUserChoice(2);
    
    switch (choice) {
        case 1: viewSettings(); break;
        case 2: updateSettings(); break;
        case 0: return;
    }
}

void UIManager::viewSettings() {
    clearScreen();
    printHeader("📋 当前设置");
    
    displayInfo("设置模块开发中...");
    
    pause();
}

void UIManager::updateSettings() {
    clearScreen();
    printHeader("✏️  修改设置");
    
    displayInfo("设置模块开发中...");
    
    pause();
}

// === 消息显示 ===

void UIManager::displayMessage(const string& msg, const string& type) {
    if (type == "success") {
        displaySuccess(msg);
    } else if (type == "error") {
        displayError(msg);
    } else if (type == "warning") {
        displayWarning(msg);
    } else {
        displayInfo(msg);
    }
}

void UIManager::displayError(const string& error) {
    cout << COLOR_RED << "❌ " << error << COLOR_RESET << "\n";
}

void UIManager::displaySuccess(const string& msg) {
    cout << COLOR_GREEN << "✅ " << msg << COLOR_RESET << "\n";
}

void UIManager::displayWarning(const string& warning) {
    cout << COLOR_YELLOW << "⚠️  " << warning << COLOR_RESET << "\n";
}

void UIManager::displayInfo(const string& info) {
    cout << COLOR_CYAN << "ℹ️  " << info << COLOR_RESET << "\n";
}
