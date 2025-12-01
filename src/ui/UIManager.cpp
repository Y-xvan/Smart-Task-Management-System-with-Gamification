/**
 * @file UIManager.cpp
 * @brief 智能任务管理系统 - UI管理器实现
 * 
 * 功能概述：
 * - 提供完整的用户界面交互
 * - 任务管理、项目管理、统计分析、游戏化功能
 * - 使用选择式输入提升用户体验
 * - 颜色选择和名称选择代替ID输入
 */

#include "ui/UIManager.h"
#include "database/DatabaseManager.h"
#include "statistics/StatisticsAnalyzer.h"
#include "gamification/XPSystem.h"
#include "HeatmapVisualizer/HeatmapVisualizer.h"
#include "project/ProjectManager.h"
#include "task/TaskManager.h"
#include "Pomodoro/pomodoro.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <thread>
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

// === ANSI颜色定义 ===
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

// === 预定义颜色选项 (名称, 十六进制代码) ===
const vector<pair<string, string>> UIManager::COLOR_OPTIONS = {
    {"红色 (Red)", "#F44336"},
    {"粉色 (Pink)", "#E91E63"},
    {"紫色 (Purple)", "#9C27B0"},
    {"深紫 (Deep Purple)", "#673AB7"},
    {"靛蓝 (Indigo)", "#3F51B5"},
    {"蓝色 (Blue)", "#2196F3"},
    {"浅蓝 (Light Blue)", "#03A9F4"},
    {"青色 (Cyan)", "#00BCD4"},
    {"蓝绿 (Teal)", "#009688"},
    {"绿色 (Green)", "#4CAF50"},
    {"浅绿 (Light Green)", "#8BC34A"},
    {"黄绿 (Lime)", "#CDDC39"},
    {"黄色 (Yellow)", "#FFEB3B"},
    {"琥珀 (Amber)", "#FFC107"},
    {"橙色 (Orange)", "#FF9800"},
    {"深橙 (Deep Orange)", "#FF5722"},
    {"棕色 (Brown)", "#795548"},
    {"灰色 (Grey)", "#9E9E9E"},
    {"蓝灰 (Blue Grey)", "#607D8B"}
};

// === 构造函数与析构函数 ===

UIManager::UIManager() {
    running = true;
    
    // 创建各模块实例
    statsAnalyzer = new StatisticsAnalyzer();
    xpSystem = new XPSystem();
    heatmap = new HeatmapVisualizer();
    projectManager = new ProjectManager();
    taskManager = new TaskManager();
    pomodoro = new Pomodoro();
    
    cout << COLOR_GREEN << "✅ UI管理器初始化成功" << COLOR_RESET << endl;
}

UIManager::~UIManager() {
    if (statsAnalyzer) delete statsAnalyzer;
    if (xpSystem) delete xpSystem;
    if (heatmap) delete heatmap;
    if (projectManager) delete projectManager;
    if (taskManager) delete taskManager;
    if (pomodoro) delete pomodoro;
}

// === UI辅助方法 ===

void UIManager::clearScreen() {
    cout << "\033[2J\033[H";
}

void UIManager::printHeader(const string& title) {
    cout << "\n";
    cout << BOLD << COLOR_CYAN;
    printSeparator("=", 55);
    cout << "    " << title << "\n";
    printSeparator("=", 55);
    cout << COLOR_RESET << "\n";
}

void UIManager::printSeparator(const string& symbol, int length) {
    for (int i = 0; i < length; i++) {
        cout << symbol;
    }
    cout << "\n";
}

void UIManager::printMenu(const vector<string>& options) {
    cout << "\n";
    for (size_t i = 0; i < options.size(); i++) {
        cout << "  " << COLOR_YELLOW << "[" << i + 1 << "]" << COLOR_RESET 
             << " " << options[i] << "\n";
    }
    cout << "  " << COLOR_RED << "[0]" << COLOR_RESET << " 返回/退出\n";
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

// === 选择式输入辅助方法 ===

/**
 * @brief 通过名称选择任务
 * @return 选中的任务ID，如果取消返回-1
 */
int UIManager::selectTaskByName() {
    auto tasks = taskManager->getAllTasks();
    
    if (tasks.empty()) {
        displayInfo("暂无任务可选择");
        return -1;
    }
    
    cout << "\n" << BOLD << "请选择任务：" << COLOR_RESET << "\n";
    printSeparator("-", 50);
    
    for (size_t i = 0; i < tasks.size(); i++) {
        string status = tasks[i].isCompleted() ? COLOR_GREEN + "[Done]" : COLOR_RED + "[Todo]";
        cout << "  " << COLOR_YELLOW << "[" << i + 1 << "]" << COLOR_RESET << " "
             << status << COLOR_RESET << " " << tasks[i].getName();
        
        // 显示优先级标记
        int prio = tasks[i].getPriority();
        if (prio == 2) cout << COLOR_RED << " ***" << COLOR_RESET;
        else if (prio == 1) cout << COLOR_YELLOW << " **" << COLOR_RESET;
        else cout << COLOR_GREEN << " *" << COLOR_RESET;
        
        cout << "\n";
    }
    
    cout << "  " << COLOR_RED << "[0]" << COLOR_RESET << " 取消\n";
    printSeparator("-", 50);
    
    int choice = getUserChoice(static_cast<int>(tasks.size()));
    
    if (choice == 0) return -1;
    return tasks[choice - 1].getId();
}

/**
 * @brief 通过名称选择项目
 * @return 选中的项目ID，如果取消返回-1
 */
int UIManager::selectProjectByName() {
    auto projects = projectManager->getAllProjects();
    
    if (projects.empty()) {
        displayInfo("暂无项目可选择");
        return -1;
    }
    
    cout << "\n" << BOLD << "请选择项目：" << COLOR_RESET << "\n";
    printSeparator("-", 50);
    
    for (size_t i = 0; i < projects.size(); i++) {
        cout << "  " << COLOR_YELLOW << "[" << i + 1 << "]" << COLOR_RESET << " "
             << COLOR_BLUE << projects[i]->getName() << COLOR_RESET;
        
        // 显示进度
        double prog = projects[i]->getProgress() * 100;
        cout << " (" << fixed << setprecision(0) << prog << "%)";
        cout << "\n";
    }
    
    cout << "  " << COLOR_RED << "[0]" << COLOR_RESET << " 取消/不分配\n";
    printSeparator("-", 50);
    
    int choice = getUserChoice(static_cast<int>(projects.size()));
    
    if (choice == 0) return -1;
    return projects[choice - 1]->getId();
}

/**
 * @brief 颜色选择
 * @return 选中的颜色十六进制代码
 */
string UIManager::selectColor() {
    cout << "\n" << BOLD << "请选择项目颜色：" << COLOR_RESET << "\n";
    printSeparator("-", 50);
    
    for (size_t i = 0; i < COLOR_OPTIONS.size(); i++) {
        cout << "  " << COLOR_YELLOW << "[" << setw(2) << i + 1 << "]" << COLOR_RESET 
             << " " << COLOR_OPTIONS[i].first << "\n";
    }
    
    cout << "  " << COLOR_RED << "[0]" << COLOR_RESET << " 使用默认颜色\n";
    printSeparator("-", 50);
    
    int choice = getUserChoice(static_cast<int>(COLOR_OPTIONS.size()));
    
    if (choice == 0) return "#4CAF50"; // 默认绿色
    return COLOR_OPTIONS[choice - 1].second;
}

/**
 * @brief 优先级选择
 * @return 选中的优先级 (0=低, 1=中, 2=高)
 */
int UIManager::selectPriority() {
    cout << "\n" << BOLD << "请选择优先级：" << COLOR_RESET << "\n";
    printSeparator("-", 30);
    cout << "  " << COLOR_GREEN << "[1]" << COLOR_RESET << " 低优先级 *\n";
    cout << "  " << COLOR_YELLOW << "[2]" << COLOR_RESET << " 中优先级 **\n";
    cout << "  " << COLOR_RED << "[3]" << COLOR_RESET << " 高优先级 ***\n";
    printSeparator("-", 30);
    
    int choice = getUserChoice(3);
    
    // 转换为0-2的优先级值
    if (choice >= 1 && choice <= 3) return choice - 1;
    return 1; // 默认中优先级
}

// === 游戏化UI增强 ===

void UIManager::printProgressBar(int current, int total, int width, string color) {
    float percentage = (total == 0) ? 0 : (float)current / total;
    if (percentage > 1.0f) percentage = 1.0f;
    int filled = static_cast<int>(width * percentage);
    
    cout << " [";
    cout << color;
    for (int i = 0; i < width; ++i) {
        // 使用Unicode进度条字符
        if (i < filled) cout << "█";  // filled block
        else cout << "░";              // light shade
    }
    cout << COLOR_RESET << "] " << int(percentage * 100) << "%";
}

void UIManager::printEncouragement() {
    static const vector<string> quotes = {
        "Keep the streak alive! 🔥", 
        "Small steps, big progress. 💪", 
        "You are unstoppable today! 🚀", 
        "Focus is the key to victory. 🎯",
        "Every task completed is a win! 🏆",
        "Building great habits! ⭐"
    };
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(0, quotes.size() - 1);

    cout << "\n" << BOLD << COLOR_YELLOW << " >> " << quotes[dis(gen)] << COLOR_RESET << "\n";
}

void UIManager::displayHUD() {
    int level = xpSystem->getCurrentLevel();
    int currentXP = xpSystem->getCurrentXP();
    int nextLevelXP = xpSystem->getXPForNextLevel(); 
    string title = xpSystem->getCurrentLevelTitle();
    int achievements = statsAnalyzer->getAchievementsUnlocked();
    int streak = statsAnalyzer->getCurrentStreak();
    
    cout << BOLD << COLOR_CYAN;
    printSeparator("─", 60);
    cout << COLOR_RESET;
    
    // 第一行：等级、称号和成就
    cout << " Lv." << level << " [" << COLOR_MAGENTA << title << COLOR_RESET << "]"
         << "    🏆 " << achievements << " 成就"
         << "    🔥 " << streak << " 天连续\n";
    
    // 第二行：经验值进度条
    cout << " XP: ";
    printProgressBar(currentXP, nextLevelXP, 35, COLOR_GREEN);
    cout << " (" << currentXP << "/" << nextLevelXP << ")\n";
    
    cout << BOLD << COLOR_CYAN;
    printSeparator("─", 60);
    cout << COLOR_RESET;
    
    printEncouragement();
}

void UIManager::showTaskCompleteCelebration(int xpGained) {
    cout << "\n";
    for(int i = 0; i < 3; ++i) {
        cout << COLOR_YELLOW << "  ★  Reward Unlocking...  ★  " << COLOR_RESET << "\r";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(150));
        cout << "                              \r"; 
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    
    cout << "\n  " << COLOR_GREEN << BOLD << "✅ TASK COMPLETED! Awesome!" << COLOR_RESET << "\n";
    cout << "  " << COLOR_YELLOW << "+" << xpGained << " XP" << COLOR_RESET << "\n\n";
    
    this_thread::sleep_for(chrono::milliseconds(800)); 
}

// === 主界面 ===

void UIManager::showMainMenu() {
    clearScreen();
    
    cout << BOLD << COLOR_BLUE;
    cout << R"(
   ╔═══════════════════════════════════════════════════╗
   ║    🎮 Smart Task Management System v2.0 🎮        ║
   ║         智能任务管理系统 - 游戏化版本             ║
   ╚═══════════════════════════════════════════════════╝
)" << COLOR_RESET;
    
    displayHUD();
    
    vector<string> options = {
        "📋 任务管理 (Task Management)",
        "📁 项目管理 (Project Management)",
        "🍅 番茄钟 (Pomodoro Timer)",
        "📊 统计分析 (Statistics)",
        "🎮 游戏化功能 (Gamification)",
        "⚙️  设置 (Settings)"
    };
    
    printMenu(options);
}

void UIManager::run() {
    cout << COLOR_GREEN << "\n🎉 欢迎使用智能任务管理系统！\n" << COLOR_RESET;
    pause();
    
    while (running) {
        showMainMenu();
        int choice = getUserChoice(6);
        
        switch (choice) {
            case 1: showTaskMenu(); break;
            case 2: showProjectMenu(); break;
            case 3: showPomodoroMenu(); break;
            case 4: showStatisticsMenu(); break;
            case 5: showGamificationMenu(); break;
            case 6: showSettingsMenu(); break;
            case 0: exitProgram(); break;
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
    printHeader("📋 任务管理 (Task Management)");
    
    vector<string> options = {
        "✨ 创建新任务 (Create Task)",
        "📋 查看所有任务 (View All Tasks)",
        "✏️  更新任务 (Update Task)",
        "🗑️  删除任务 (Delete Task)",
        "✅ 完成任务 (Complete Task) 🎯",
        "📎 分配任务到项目 (Assign to Project)"
    };
    
    printMenu(options);
    int choice = getUserChoice(6);
    
    switch (choice) {
        case 1: createTask(); break;
        case 2: listTasks(); break;
        case 3: updateTask(); break;
        case 4: deleteTask(); break;
        case 5: completeTask(); break;
        case 6: assignTaskToProject(); break;
        case 0: return;
    }
}

void UIManager::createTask() {
    clearScreen();
    printHeader("✨ 创建新任务 (Create New Task)");

    // 基本信息
    string name = getInput("📝 任务名称 (Task Name): ");
    if (name.empty()) {
        displayError("任务名称不能为空！");
        pause();
        return;
    }
    
    string desc = getInput("📄 任务描述 (Description，可选): ");
    
    // 选择优先级
    int priority = selectPriority();
    
    // 截止日期
    string due = getInput("📅 截止日期 (Due Date YYYY-MM-DD，直接回车跳过): ");
    
    // 标签
    string tags = getInput("🏷️  标签 (Tags，用逗号分隔，直接回车跳过): ");
    
    // 预计番茄数
    cout << "\n🍅 预计番茄数 (Estimated Pomodoros，每个25分钟，输入0跳过)\n";
    int estimated = getIntInput("   Pomodoros: ");
    
    // 提醒时间
    string reminder = getInput("⏰ 提醒时间 (Reminder Time YYYY-MM-DD HH:MM，直接回车跳过): ");
    
    // 是否分配到项目
    int projectId = -1;
    if (confirmAction("\n是否分配到项目？")) {
        projectId = selectProjectByName();
    }

    // 创建任务对象
    Task t(name, desc);
    t.setPriority(priority);
    if (!due.empty()) t.setDueDate(due);
    if (!tags.empty()) t.setTags(tags);
    if (estimated > 0) t.setEstimatedPomodoros(estimated);
    if (!reminder.empty()) t.setReminderTime(reminder);
    if (projectId > 0) t.setProjectId(projectId);

    int id = taskManager->createTask(t);

    if (id > 0) {
        displaySuccess("任务创建成功！ID = " + to_string(id));
        
        // 显示任务摘要
        cout << "\n" << BOLD << "📋 任务摘要 (Task Summary)：" << COLOR_RESET << "\n";
        cout << "  Name: " << name << "\n";
        cout << "  Priority: ";
        if (priority == 2) cout << COLOR_RED << "High ***" << COLOR_RESET;
        else if (priority == 1) cout << COLOR_YELLOW << "Medium **" << COLOR_RESET;
        else cout << COLOR_GREEN << "Low *" << COLOR_RESET;
        cout << "\n";
        if (!due.empty()) cout << "  Due Date: " << due << "\n";
    } else {
        displayError("创建失败，请重试。");
    }

    pause();
}

void UIManager::listTasks() {
    clearScreen();
    printHeader("📋 任务列表 (Task List)");
    
    auto tasks = taskManager->getAllTasks();
    if (tasks.empty()) {
        displayInfo("暂无任务。赶快创建一个吧！");
        pause();
        return;
    }
    
    // 统计信息
    int total = tasks.size();
    int completed = 0;
    for (const auto& t : tasks) {
        if (t.isCompleted()) completed++;
    }
    
    cout << "\n" << COLOR_CYAN << "📊 统计: " << COLOR_RESET 
         << completed << "/" << total << " 已完成 ("
         << fixed << setprecision(0) << (total > 0 ? (completed * 100.0 / total) : 0) << "%)\n";
    
    printSeparator("-", 60);
    
    // 分类显示：未完成 -> 已完成
    cout << "\n" << BOLD << "⏳ 进行中的任务：" << COLOR_RESET << "\n";
    bool hasPending = false;
    for (const auto& t : tasks) {
        if (!t.isCompleted()) {
            hasPending = true;
            cout << "  ";
            
            // 优先级图标
            int prio = t.getPriority();
            if (prio == 2) cout << COLOR_RED << "🔴";
            else if (prio == 1) cout << COLOR_YELLOW << "🟡";
            else cout << COLOR_GREEN << "🟢";
            cout << COLOR_RESET;
            
            cout << " " << t.getName();
            
            // 显示截止日期
            if (!t.getDueDate().empty()) {
                cout << COLOR_CYAN << " (📅 " << t.getDueDate() << ")" << COLOR_RESET;
            }
            
            // 显示番茄钟进度
            if (t.getEstimatedPomodoros() > 0) {
                cout << " 🍅" << t.getPomodoroCount() << "/" << t.getEstimatedPomodoros();
            }
            
            cout << "\n";
        }
    }
    if (!hasPending) {
        cout << "  " << COLOR_GREEN << "🎉 太棒了！没有待完成的任务！" << COLOR_RESET << "\n";
    }
    
    cout << "\n" << BOLD << "✅ 已完成的任务：" << COLOR_RESET << "\n";
    bool hasCompleted = false;
    for (const auto& t : tasks) {
        if (t.isCompleted()) {
            hasCompleted = true;
            cout << "  " << COLOR_GREEN << "✔ " << t.getName() << COLOR_RESET << "\n";
        }
    }
    if (!hasCompleted) {
        cout << "  " << COLOR_YELLOW << "暂无已完成任务" << COLOR_RESET << "\n";
    }
    
    printSeparator("-", 60);
    pause();
}

void UIManager::updateTask() {
    clearScreen();
    printHeader("✏️  更新任务 (Update Task)");
    
    // 使用选择式输入
    int taskId = selectTaskByName();
    if (taskId < 0) {
        pause();
        return;
    }

    auto opt = taskManager->getTask(taskId);
    if (!opt.has_value()) {
        displayError("任务不存在！");
        pause();
        return;
    }

    Task task = opt.value();
    
    cout << "\n" << BOLD << "📋 当前任务信息：" << COLOR_RESET << "\n";
    cout << "  名称: " << task.getName() << "\n";
    cout << "  描述: " << (task.getDescription().empty() ? "(无)" : task.getDescription()) << "\n";
    cout << "  优先级: " << task.getPriority() << "\n";
    cout << "  状态: " << (task.isCompleted() ? "已完成" : "未完成") << "\n";
    
    cout << "\n" << BOLD << "选择要修改的内容：" << COLOR_RESET << "\n";
    printSeparator("-", 40);
    cout << "  " << COLOR_YELLOW << "[1]" << COLOR_RESET << " 📝 修改名称\n";
    cout << "  " << COLOR_YELLOW << "[2]" << COLOR_RESET << " 📄 修改描述\n";
    cout << "  " << COLOR_YELLOW << "[3]" << COLOR_RESET << " ⭐ 修改优先级\n";
    cout << "  " << COLOR_YELLOW << "[4]" << COLOR_RESET << " 📅 修改截止日期\n";
    cout << "  " << COLOR_YELLOW << "[5]" << COLOR_RESET << " 🔄 切换完成状态\n";
    cout << "  " << COLOR_RED << "[0]" << COLOR_RESET << " ❌ 取消\n";
    printSeparator("-", 40);
    
    int choice = getUserChoice(5);
    
    switch (choice) {
        case 1: {
            string newName = getInput("📝 新名称: ");
            if (!newName.empty()) task.setName(newName);
            break;
        }
        case 2: {
            string newDesc = getInput("📄 新描述: ");
            task.setDescription(newDesc);
            break;
        }
        case 3: {
            task.setPriority(selectPriority());
            break;
        }
        case 4: {
            string newDue = getInput("📅 新截止日期 (YYYY-MM-DD): ");
            task.setDueDate(newDue);
            break;
        }
        case 5: {
            task.setCompleted(!task.isCompleted());
            break;
        }
        case 0:
            return;
    }

    if (taskManager->updateTask(task)) {
        displaySuccess("任务更新成功！");
    } else {
        displayError("更新失败！");
    }

    pause();
}

void UIManager::deleteTask() {
    clearScreen();
    printHeader("🗑️  删除任务 (Delete Task)");
    
    // 使用选择式输入
    int taskId = selectTaskByName();
    if (taskId < 0) {
        pause();
        return;
    }
    
    auto opt = taskManager->getTask(taskId);
    if (opt.has_value()) {
        cout << "\n" << COLOR_YELLOW << "⚠️  即将删除任务: " << opt.value().getName() << COLOR_RESET << "\n";
    }
    
    if (confirmAction("确定要删除这个任务吗？")) {
        if (taskManager->deleteTask(taskId)) {
            displaySuccess("任务已删除。");
        } else {
            displayError("删除失败。");
        }
    } else {
        displayInfo("已取消删除操作。");
    }
    pause();
}

void UIManager::completeTask() {
    clearScreen();
    printHeader("✅ 完成任务 (Complete Task)");
    
    auto tasks = taskManager->getAllTasks();
    
    // 筛选未完成的任务
    vector<Task> pendingTasks;
    for (const auto& t : tasks) {
        if (!t.isCompleted()) {
            pendingTasks.push_back(t);
        }
    }
    
    if (pendingTasks.empty()) {
        displayInfo("🎉 太棒了！没有待完成的任务！");
        pause();
        return;
    }
    
    cout << "\n" << BOLD << "选择要完成的任务：" << COLOR_RESET << "\n";
    printSeparator("-", 50);
    
    for (size_t i = 0; i < pendingTasks.size(); i++) {
        cout << "  " << COLOR_YELLOW << "[" << i + 1 << "]" << COLOR_RESET << " "
             << pendingTasks[i].getName();
        
        int prio = pendingTasks[i].getPriority();
        if (prio == 2) cout << COLOR_RED << " ★★★" << COLOR_RESET;
        else if (prio == 1) cout << COLOR_YELLOW << " ★★" << COLOR_RESET;
        else cout << COLOR_GREEN << " ★" << COLOR_RESET;
        
        cout << "\n";
    }
    
    cout << "  " << COLOR_RED << "[0]" << COLOR_RESET << " ❌ 取消\n";
    printSeparator("-", 50);
    
    int choice = getUserChoice(static_cast<int>(pendingTasks.size()));
    
    if (choice == 0) return;
    
    int taskId = pendingTasks[choice - 1].getId();
    int priority = pendingTasks[choice - 1].getPriority();
    
    if (taskManager->completeTask(taskId)) {
        int xpReward = xpSystem->getXPForTaskCompletion(priority);
        xpSystem->awardXP(xpReward, "完成任务: " + pendingTasks[choice - 1].getName());
        showTaskCompleteCelebration(xpReward);
    } else {
        displayError("操作失败！");
        pause();
    }
}

void UIManager::assignTaskToProject() {
    clearScreen();
    printHeader("📎 分配任务到项目 (Assign Task to Project)");
    
    cout << "\n" << BOLD << "📌 步骤1: 选择任务" << COLOR_RESET << "\n";
    int taskId = selectTaskByName();
    if (taskId < 0) {
        pause();
        return;
    }
    
    cout << "\n" << BOLD << "📌 步骤2: 选择项目" << COLOR_RESET << "\n";
    int projectId = selectProjectByName();
    
    if (projectId < 0) {
        displayInfo("已取消分配。");
        pause();
        return;
    }
    
    if (taskManager->assignTaskToProject(taskId, projectId)) {
        displaySuccess("🎉 任务已成功分配到项目！");
    } else {
        displayError("分配失败！");
    }
    
    pause();
}

// === 项目管理界面 ===

void UIManager::showProjectMenu() {
    clearScreen();
    printHeader("📁 项目管理 (Project Management)");
    
    vector<string> options = {
        "✨ 创建新项目 (Create Project)",
        "📁 查看所有项目 (View All Projects)",
        "📊 查看项目详情 (Project Details)",
        "✏️  更新项目 (Update Project)",
        "🗑️  删除项目 (Delete Project)"
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
    printHeader("✨ 创建新项目 (Create New Project)");
    
    string name = getInput("📝 项目名称: ");
    if (name.empty()) {
        displayError("项目名称不能为空！");
        pause();
        return;
    }
    
    string desc = getInput("📄 项目描述: ");
    
    // 使用颜色选择替代手动输入
    string color = selectColor();
    
    Project project(name, desc, color);
    int id = projectManager->createProject(project);
    
    if (id > 0) {
        displaySuccess("🎉 项目创建成功！ID: " + to_string(id));
        
        cout << "\n" << BOLD << "📋 项目摘要：" << COLOR_RESET << "\n";
        cout << "  名称: " << name << "\n";
        cout << "  描述: " << (desc.empty() ? "(无)" : desc) << "\n";
        cout << "  颜色: " << color << "\n";
    } else {
        displayError("创建失败！");
    }
    
    pause();
}

void UIManager::listProjects() {
    clearScreen();
    printHeader("📁 项目列表 (Project List)");
    
    vector<Project*> projects = projectManager->getAllProjects();
    
    if (projects.empty()) {
        displayInfo("暂无项目，赶快创建一个吧！");
        pause();
        return;
    }
    
    cout << "\n";
    printSeparator("-", 55);
    
    for (Project* p : projects) {
        cout << "\n  " << COLOR_BLUE << BOLD << "📁 " << p->getName() << COLOR_RESET << "\n";
        cout << "  " << "📄 描述: " << (p->getDescription().empty() ? "(无)" : p->getDescription()) << "\n";
        
        // 进度条
        double prog = p->getProgress();
        cout << "  📊 进度: ";
        printProgressBar(static_cast<int>(prog * 100), 100, 20, COLOR_GREEN);
        cout << " (" << p->getCompletedTasks() << "/" << p->getTotalTasks() << " 任务)\n";
        
        printSeparator("-", 55);
    }
    
    pause();
}

void UIManager::viewProjectDetails() {
    clearScreen();
    printHeader("📊 项目详情 (Project Details)");
    
    // 使用选择式输入
    int projectId = selectProjectByName();
    if (projectId < 0) {
        pause();
        return;
    }
    
    Project* p = projectManager->getProject(projectId);
    
    if (p == nullptr) {
        displayError("项目不存在！");
        pause();
        return;
    }
    
    cout << "\n";
    cout << BOLD << "╔═══════════════════════════════════════╗\n";
    cout << "║  📁 项目: " << p->getName() << "\n";
    cout << "╚═══════════════════════════════════════╝" << COLOR_RESET << "\n\n";
    
    cout << "📄 描述: " << (p->getDescription().empty() ? "(无)" : p->getDescription()) << "\n";
    cout << "🎨 颜色: " << p->getColorLabel() << "\n";
    cout << "📅 目标日期: " << (p->getTargetDate().empty() ? "(未设置)" : p->getTargetDate()) << "\n";
    cout << "📆 创建日期: " << p->getCreatedDate() << "\n";
    cout << "📊 状态: " << (p->isArchived() ? "已归档 📦" : "活跃中 ✅") << "\n";
    
    cout << "\n" << BOLD << "📈 任务统计:" << COLOR_RESET << "\n";
    cout << "  总任务: " << p->getTotalTasks() << "\n";
    cout << "  已完成: " << p->getCompletedTasks() << "\n";
    cout << "  进度: ";
    printProgressBar(static_cast<int>(p->getProgress() * 100), 100, 25, COLOR_GREEN);
    cout << "\n";
    
    // 显示项目下的任务
    auto tasks = taskManager->getTasksByProject(projectId);
    if (!tasks.empty()) {
        cout << "\n" << BOLD << "📋 项目任务列表:" << COLOR_RESET << "\n";
        for (const auto& t : tasks) {
            string status = t.isCompleted() ? COLOR_GREEN + "✅" : COLOR_YELLOW + "⏳";
            cout << "  " << status << COLOR_RESET << " " << t.getName() << "\n";
        }
    }
    
    pause();
}

void UIManager::updateProject() {
    clearScreen();
    printHeader("✏️  更新项目 (Update Project)");
    
    // 使用选择式输入
    int projectId = selectProjectByName();
    if (projectId < 0) {
        pause();
        return;
    }
    
    Project* p = projectManager->getProject(projectId);
    if (p == nullptr) {
        displayError("项目不存在！");
        pause();
        return;
    }
    
    cout << "\n" << BOLD << "📋 当前项目信息：" << COLOR_RESET << "\n";
    cout << "  名称: " << p->getName() << "\n";
    cout << "  描述: " << p->getDescription() << "\n";
    cout << "  颜色: " << p->getColorLabel() << "\n";
    
    cout << "\n" << BOLD << "选择要修改的内容：" << COLOR_RESET << "\n";
    printSeparator("-", 40);
    cout << "  " << COLOR_YELLOW << "[1]" << COLOR_RESET << " 📝 修改名称\n";
    cout << "  " << COLOR_YELLOW << "[2]" << COLOR_RESET << " 📄 修改描述\n";
    cout << "  " << COLOR_YELLOW << "[3]" << COLOR_RESET << " 🎨 修改颜色\n";
    cout << "  " << COLOR_YELLOW << "[4]" << COLOR_RESET << " 📅 设置目标日期\n";
    cout << "  " << COLOR_YELLOW << "[5]" << COLOR_RESET << " 📦 归档/取消归档\n";
    cout << "  " << COLOR_RED << "[0]" << COLOR_RESET << " ❌ 取消\n";
    printSeparator("-", 40);
    
    int choice = getUserChoice(5);
    
    switch (choice) {
        case 1: {
            string newName = getInput("📝 新名称: ");
            if (!newName.empty()) p->setName(newName);
            break;
        }
        case 2: {
            string newDesc = getInput("📄 新描述: ");
            p->setDescription(newDesc);
            break;
        }
        case 3: {
            string newColor = selectColor();
            p->setColorLabel(newColor);
            break;
        }
        case 4: {
            string newDate = getInput("📅 目标日期 (YYYY-MM-DD): ");
            p->setTargetDate(newDate);
            break;
        }
        case 5: {
            p->setArchived(!p->isArchived());
            displayInfo(p->isArchived() ? "📦 项目已归档" : "✅ 项目已取消归档");
            break;
        }
        case 0:
            return;
    }
    
    if (projectManager->updateProject(*p)) {
        displaySuccess("🎉 项目更新成功！");
    } else {
        displayError("更新失败！");
    }
    
    pause();
}

void UIManager::deleteProject() {
    clearScreen();
    printHeader("🗑️  删除项目 (Delete Project)");
    
    // 使用选择式输入
    int projectId = selectProjectByName();
    if (projectId < 0) {
        pause();
        return;
    }
    
    Project* p = projectManager->getProject(projectId);
    if (p != nullptr) {
        cout << "\n" << COLOR_YELLOW << "⚠️  即将删除项目: " << p->getName() << COLOR_RESET << "\n";
    }
    
    if (confirmAction("确定要删除这个项目吗？（项目下的任务不会被删除）")) {
        if (projectManager->deleteProject(projectId)) {
            displaySuccess("🎉 项目删除成功！");
        } else {
            displayError("删除失败！");
        }
    } else {
        displayInfo("已取消删除操作。");
    }
    
    pause();
}

// === 番茄钟功能 ===

void UIManager::showPomodoroMenu() {
    clearScreen();
    printHeader("🍅 番茄钟 (Pomodoro Timer)");
    
    cout << "\n" << BOLD << "🍅 什么是番茄工作法？" << COLOR_RESET << "\n";
    cout << "  专注工作一段时间，然后休息一下。\n";
    cout << "  每完成4个番茄钟，可以休息更长时间。\n";
    
    cout << "\n" << COLOR_CYAN << "📊 今日番茄钟: " << COLOR_RESET 
         << statsAnalyzer->getPomodorosToday() << " 个\n";
    cout << COLOR_CYAN << "📈 累计番茄钟: " << COLOR_RESET 
         << pomodoro->getCycleCount() << " 个\n";
    
    cout << "\n" << BOLD << "⚙️  当前设置:" << COLOR_RESET << "\n";
    cout << "  工作: " << pomodoro->getWorkDuration() << "分钟 | ";
    cout << "短休息: " << pomodoro->getBreakDuration() << "分钟 | ";
    cout << "长休息: " << pomodoro->getLongBreakDuration() << "分钟\n";
    
    vector<string> options = {
        "🍅 开始工作 (" + to_string(pomodoro->getWorkDuration()) + "分钟)",
        "☕ 短休息 (" + to_string(pomodoro->getBreakDuration()) + "分钟)",
        "🛋️  长休息 (" + to_string(pomodoro->getLongBreakDuration()) + "分钟)"
    };
    
    printMenu(options);
    int choice = getUserChoice(3);
    
    switch (choice) {
        case 1: startPomodoroSession(); break;
        case 2: startShortBreak(); break;
        case 3: startLongBreak(); break;
        case 0: return;
    }
}

void UIManager::startPomodoroSession() {
    clearScreen();
    printHeader("🍅 番茄钟工作时间");
    
    // 选择关联的任务（可选）
    cout << "\n是否要关联一个任务？（完成后任务的番茄数会+1）\n";
    int taskId = -1;
    if (confirmAction("关联任务？")) {
        taskId = selectTaskByName();
    }
    
    int duration = pomodoro->getWorkDuration();
    cout << "\n" << COLOR_GREEN << BOLD << "🍅 番茄钟开始！" << COLOR_RESET << "\n";
    cout << "  ⏱️  专注时间: " << duration << "分钟\n";
    cout << "  🎯 保持专注，减少干扰！\n";
    cout << "  💡 按 Ctrl+C 可中断计时\n\n";
    
    // 使用真实倒计时
    cout << BOLD << "⏱️  倒计时: " << COLOR_RESET;
    cout.flush();
    
    bool completed = pomodoro->startWorkWithCountdown([this, duration](int remaining) {
        int mins = remaining / 60;
        int secs = remaining % 60;
        
        // 计算进度百分比
        int total = duration * 60;
        int elapsed = total - remaining;
        int percent = (elapsed * 100) / total;
        
        // 清除当前行并重新显示
        cout << "\r" << BOLD << "⏱️  倒计时: " << COLOR_RESET;
        cout << COLOR_CYAN << setfill('0') << setw(2) << mins << ":" 
             << setfill('0') << setw(2) << secs << COLOR_RESET;
        cout << "  [";
        
        // 进度条 (20字符宽)
        int filled = (percent * 20) / 100;
        cout << COLOR_GREEN;
        for (int i = 0; i < 20; i++) {
            if (i < filled) cout << "█";
            else cout << "░";
        }
        cout << COLOR_RESET << "] " << percent << "%  ";
        cout.flush();
    });
    
    cout << "\n\n";
    
    if (completed) {
        // 番茄钟完成
        cout << COLOR_GREEN << BOLD << "🎉 番茄钟完成！" << COLOR_RESET << "\n";
        
        // 奖励XP
        int xpReward = xpSystem->getXPForPomodoro();
        xpSystem->awardXP(xpReward, "完成番茄钟");
        cout << "  +" << COLOR_YELLOW << xpReward << " XP" << COLOR_RESET << "\n";
        
        // 如果关联了任务，增加任务的番茄数
        if (taskId > 0) {
            taskManager->addPomodoro(taskId);
            displaySuccess("🍅 任务番茄数 +1");
        }
        
        cout << "\n  " << COLOR_CYAN << "☕ 休息一下吧！" << COLOR_RESET << "\n";
        
        // 显示今日番茄钟统计
        cout << "\n  📊 今日完成番茄钟: " << statsAnalyzer->getPomodorosToday() << " 个\n";
        cout << "  🍅 累计完成番茄钟: " << pomodoro->getCycleCount() << " 个\n";
    } else {
        displayWarning("番茄钟被中断");
    }
    
    pause();
}

void UIManager::startShortBreak() {
    clearScreen();
    printHeader("☕ 短休息时间");
    
    int duration = pomodoro->getBreakDuration();
    cout << "\n" << COLOR_CYAN << BOLD << "☕ 短休息开始！" << COLOR_RESET << "\n";
    cout << "  ⏱️  休息时间: " << duration << "分钟\n";
    cout << "  💡 站起来活动一下，喝杯水！\n\n";
    
    // 使用真实倒计时
    cout << BOLD << "⏱️  倒计时: " << COLOR_RESET;
    cout.flush();
    
    bool completed = pomodoro->startBreakWithCountdown([this, duration](int remaining) {
        int mins = remaining / 60;
        int secs = remaining % 60;
        int total = duration * 60;
        int elapsed = total - remaining;
        int percent = (elapsed * 100) / total;
        
        cout << "\r" << BOLD << "⏱️  倒计时: " << COLOR_RESET;
        cout << COLOR_CYAN << setfill('0') << setw(2) << mins << ":" 
             << setfill('0') << setw(2) << secs << COLOR_RESET;
        cout << "  [";
        cout << COLOR_BLUE;
        int filled = (percent * 20) / 100;
        for (int i = 0; i < 20; i++) {
            if (i < filled) cout << "█";
            else cout << "░";
        }
        cout << COLOR_RESET << "] " << percent << "%  ";
        cout.flush();
    });
    
    cout << "\n\n";
    
    if (completed) {
        displaySuccess("☕ 休息结束！准备好继续工作了吗？");
    } else {
        displayWarning("休息被中断");
    }
    
    pause();
}

void UIManager::startLongBreak() {
    clearScreen();
    printHeader("🛋️  长休息时间");
    
    int duration = pomodoro->getLongBreakDuration();
    cout << "\n" << COLOR_MAGENTA << BOLD << "🛋️  长休息开始！" << COLOR_RESET << "\n";
    cout << "  ⏱️  休息时间: " << duration << "分钟\n";
    cout << "  💡 可以出去走走，放松一下眼睛！\n\n";
    
    // 使用真实倒计时
    cout << BOLD << "⏱️  倒计时: " << COLOR_RESET;
    cout.flush();
    
    bool completed = pomodoro->startLongBreakWithCountdown([this, duration](int remaining) {
        int mins = remaining / 60;
        int secs = remaining % 60;
        int total = duration * 60;
        int elapsed = total - remaining;
        int percent = (elapsed * 100) / total;
        
        cout << "\r" << BOLD << "⏱️  倒计时: " << COLOR_RESET;
        cout << COLOR_MAGENTA << setfill('0') << setw(2) << mins << ":" 
             << setfill('0') << setw(2) << secs << COLOR_RESET;
        cout << "  [";
        cout << COLOR_MAGENTA;
        int filled = (percent * 20) / 100;
        for (int i = 0; i < 20; i++) {
            if (i < filled) cout << "█";
            else cout << "░";
        }
        cout << COLOR_RESET << "] " << percent << "%  ";
        cout.flush();
    });
    
    cout << "\n\n";
    
    if (completed) {
        displaySuccess("🛋️  休息结束！充好电了，继续加油！");
    } else {
        displayWarning("休息被中断");
    }
    
    pause();
}

// === 统计分析界面 ===

void UIManager::showStatisticsMenu() {
    clearScreen();
    printHeader("📊 统计分析 (Statistics)");
    
    vector<string> options = {
        "📈 统计数据总览 (Summary)",
        "📅 每日报告 (Daily Report)",
        "📆 每周报告 (Weekly Report)",
        "📊 每月报告 (Monthly Report)",
        "🔥 任务完成热力图 (Task Heatmap)"
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
    printHeader("📆 每周报告");
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
    cout << heatmap->generateHeatmap(90);
    pause();
}

// === 游戏化界面 ===

void UIManager::showGamificationMenu() {
    clearScreen();
    printHeader("🎮 游戏化功能 (Gamification)");
    
    vector<string> options = {
        "⭐ 经验值和等级 (XP & Level)",
        "🏆 成就系统 (Achievements)",
        "🎯 挑战系统 (Challenges)"
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
    
    cout << "\n" << BOLD << "💡 经验值获取方式：" << COLOR_RESET << "\n";
    cout << "  • 完成任务: " << COLOR_GREEN << "10-50 XP" << COLOR_RESET << " (根据优先级)\n";
    cout << "  • 完成番茄钟: " << COLOR_GREEN << "5 XP" << COLOR_RESET << "\n";
    cout << "  • 连续打卡: " << COLOR_GREEN << "天数×10 XP" << COLOR_RESET << "\n";
    cout << "  • 解锁成就: " << COLOR_GREEN << "不等" << COLOR_RESET << "\n";
    
    pause();
}

void UIManager::showAchievements() {
    clearScreen();
    printHeader("🏆 成就系统");
    
    int unlocked = statsAnalyzer->getAchievementsUnlocked();
    int totalTasks = statsAnalyzer->getTotalTasksCompleted();
    int streak = statsAnalyzer->getCurrentStreak();
    int totalPomodoros = statsAnalyzer->getTotalPomodoros();
    int todayTasks = statsAnalyzer->getTasksCompletedToday();
    
    // 成就定义（与后台AchievementManager一致）
    const int TOTAL_ACHIEVEMENTS = 4;
    
    cout << "\n" << BOLD << "🏆 成就进度: " << COLOR_RESET;
    printProgressBar(unlocked, TOTAL_ACHIEVEMENTS, 20, COLOR_YELLOW);
    cout << " (" << unlocked << "/" << TOTAL_ACHIEVEMENTS << ")\n\n";
    
    // 显示成就列表（带实际进度）
    cout << BOLD << "可用成就：" << COLOR_RESET << "\n";
    printSeparator("-", 60);
    
    // 成就1: 初次起步 - 完成第一个任务
    bool ach1 = totalTasks >= 1;
    cout << (ach1 ? COLOR_GREEN + "✅" : COLOR_YELLOW + "🔒") << COLOR_RESET
         << " 初次起步 - 完成第一个任务";
    if (!ach1) cout << " [进度: " << totalTasks << "/1]";
    cout << " +" << COLOR_GREEN << "10XP" << COLOR_RESET << "\n";
    
    // 成就2: 七日坚持 - 连续7天完成任务
    bool ach2 = streak >= 7;
    cout << (ach2 ? COLOR_GREEN + "✅" : COLOR_YELLOW + "🔒") << COLOR_RESET
         << " 七日坚持 - 连续7天完成任务";
    if (!ach2) cout << " [进度: " << streak << "/7天]";
    cout << " +" << COLOR_GREEN << "50XP" << COLOR_RESET << "\n";
    
    // 成就3: 番茄大师 - 完成100个番茄钟
    bool ach3 = totalPomodoros >= 100;
    cout << (ach3 ? COLOR_GREEN + "✅" : COLOR_YELLOW + "🔒") << COLOR_RESET
         << " 番茄大师 - 完成100个番茄钟";
    if (!ach3) cout << " [进度: " << totalPomodoros << "/100]";
    cout << " +" << COLOR_GREEN << "100XP" << COLOR_RESET << "\n";
    
    // 成就4: 时间管理大师 - 单日完成5个任务
    bool ach4 = todayTasks >= 5;
    cout << (ach4 ? COLOR_GREEN + "✅" : COLOR_YELLOW + "🔒") << COLOR_RESET
         << " 时间管理大师 - 单日完成5个任务";
    if (!ach4) cout << " [今日: " << todayTasks << "/5]";
    cout << " +" << COLOR_GREEN << "30XP" << COLOR_RESET << "\n";
    
    printSeparator("-", 60);
    
    // 显示统计信息
    cout << "\n" << BOLD << "📊 成就统计：" << COLOR_RESET << "\n";
    cout << "  已解锁: " << COLOR_GREEN << unlocked << COLOR_RESET << " 个\n";
    cout << "  未解锁: " << COLOR_YELLOW << (TOTAL_ACHIEVEMENTS - unlocked) << COLOR_RESET << " 个\n";
    cout << "  完成率: " << (unlocked * 100 / TOTAL_ACHIEVEMENTS) << "%\n";
    
    pause();
}

void UIManager::showChallenges() {
    clearScreen();
    printHeader("🎯 挑战系统");
    
    int completed = statsAnalyzer->getChallengesCompleted();
    int todayTasks = statsAnalyzer->getTasksCompletedToday();
    int todayPomodoros = statsAnalyzer->getPomodorosToday();
    int weeklyTasks = statsAnalyzer->getTotalTasksCompleted(); // 简化处理
    int streak = statsAnalyzer->getCurrentStreak();
    
    cout << "\n" << BOLD << "🏅 已完成挑战: " << COLOR_RESET 
         << COLOR_GREEN << completed << COLOR_RESET << " 个\n\n";
    
    cout << BOLD << "📅 每日挑战：" << COLOR_RESET << "\n";
    printSeparator("-", 60);
    
    // 每日挑战1: 完成3个任务
    bool daily1 = todayTasks >= 3;
    cout << (daily1 ? COLOR_GREEN + "✅" : COLOR_YELLOW + "⏳") << COLOR_RESET
         << " 今日目标 - 完成3个任务";
    cout << " [" << todayTasks << "/3]";
    cout << " 奖励: +" << COLOR_GREEN << "30XP" << COLOR_RESET << "\n";
    
    // 每日挑战2: 完成4个番茄钟
    bool daily2 = todayPomodoros >= 4;
    cout << (daily2 ? COLOR_GREEN + "✅" : COLOR_YELLOW + "⏳") << COLOR_RESET
         << " 番茄达人 - 完成4个番茄钟";
    cout << " [" << todayPomodoros << "/4]";
    cout << " 奖励: +" << COLOR_GREEN << "20XP" << COLOR_RESET << "\n";
    
    cout << "\n" << BOLD << "📆 每周挑战：" << COLOR_RESET << "\n";
    printSeparator("-", 60);
    
    // 每周挑战1: 完成15个任务
    bool weekly1 = weeklyTasks >= 15;
    cout << (weekly1 ? COLOR_GREEN + "✅" : COLOR_YELLOW + "⏳") << COLOR_RESET
         << " 周计划王 - 完成15个任务";
    cout << " [" << min(weeklyTasks, 15) << "/15]";
    cout << " 奖励: +" << COLOR_GREEN << "100XP" << COLOR_RESET << "\n";
    
    // 每周挑战2: 连续7天完成任务
    bool weekly2 = streak >= 7;
    cout << (weekly2 ? COLOR_GREEN + "✅" : COLOR_YELLOW + "⏳") << COLOR_RESET
         << " 连续作战 - 连续7天有任务完成";
    cout << " [" << streak << "/7天]";
    cout << " 奖励: +" << COLOR_GREEN << "70XP" << COLOR_RESET << "\n";
    
    printSeparator("-", 60);
    
    // 显示统计
    int dailyDone = (daily1 ? 1 : 0) + (daily2 ? 1 : 0);
    int weeklyDone = (weekly1 ? 1 : 0) + (weekly2 ? 1 : 0);
    cout << "\n" << BOLD << "📊 挑战统计：" << COLOR_RESET << "\n";
    cout << "  每日挑战: " << dailyDone << "/2 完成\n";
    cout << "  每周挑战: " << weeklyDone << "/2 完成\n";
    
    pause();
}

// === 设置界面 ===

void UIManager::showSettingsMenu() {
    clearScreen();
    printHeader("⚙️  系统设置 (Settings)");
    
    vector<string> options = {
        "📋 查看当前设置",
        "🍅 修改番茄钟时长"
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
    
    cout << "\n" << BOLD << "🍅 番茄钟设置：" << COLOR_RESET << "\n";
    cout << "  工作时长: " << COLOR_CYAN << pomodoro->getWorkDuration() << " 分钟" << COLOR_RESET << "\n";
    cout << "  短休息: " << COLOR_CYAN << pomodoro->getBreakDuration() << " 分钟" << COLOR_RESET << "\n";
    cout << "  长休息: " << COLOR_CYAN << pomodoro->getLongBreakDuration() << " 分钟" << COLOR_RESET << "\n";
    cout << "  已完成番茄钟: " << COLOR_GREEN << pomodoro->getCycleCount() << " 个" << COLOR_RESET << "\n";
    
    cout << "\n" << BOLD << "🎨 界面设置：" << COLOR_RESET << "\n";
    cout << "  主题: 默认\n";
    cout << "  语言: 中文\n";
    
    pause();
}

void UIManager::updateSettings() {
    clearScreen();
    printHeader("✏️  修改番茄钟设置");
    
    cout << "\n" << BOLD << "当前设置：" << COLOR_RESET << "\n";
    cout << "  [1] 🍅 工作时长: " << pomodoro->getWorkDuration() << " 分钟\n";
    cout << "  [2] ☕ 短休息: " << pomodoro->getBreakDuration() << " 分钟\n";
    cout << "  [3] 🛋️  长休息: " << pomodoro->getLongBreakDuration() << " 分钟\n";
    
    cout << "\n" << BOLD << "选择要修改的设置：" << COLOR_RESET << "\n";
    printSeparator("-", 40);
    cout << "  " << COLOR_YELLOW << "[1]" << COLOR_RESET << " 🍅 番茄钟工作时长 (1-120分钟)\n";
    cout << "  " << COLOR_YELLOW << "[2]" << COLOR_RESET << " ☕ 短休息时长 (1-60分钟)\n";
    cout << "  " << COLOR_YELLOW << "[3]" << COLOR_RESET << " 🛋️  长休息时长 (1-60分钟)\n";
    cout << "  " << COLOR_RED << "[0]" << COLOR_RESET << " ❌ 返回\n";
    printSeparator("-", 40);
    
    int choice = getUserChoice(3);
    
    switch (choice) {
        case 1: {
            cout << "\n当前工作时长: " << pomodoro->getWorkDuration() << " 分钟\n";
            int newDuration = getIntInput("请输入新的工作时长 (1-120分钟): ");
            if (newDuration >= 1 && newDuration <= 120) {
                pomodoro->setWorkDuration(newDuration);
                displaySuccess("工作时长已更新为 " + to_string(newDuration) + " 分钟");
            } else {
                displayError("无效的时长，请输入1-120之间的数字");
            }
            break;
        }
        case 2: {
            cout << "\n当前短休息时长: " << pomodoro->getBreakDuration() << " 分钟\n";
            int newDuration = getIntInput("请输入新的短休息时长 (1-60分钟): ");
            if (newDuration >= 1 && newDuration <= 60) {
                pomodoro->setBreakDuration(newDuration);
                displaySuccess("短休息时长已更新为 " + to_string(newDuration) + " 分钟");
            } else {
                displayError("无效的时长，请输入1-60之间的数字");
            }
            break;
        }
        case 3: {
            cout << "\n当前长休息时长: " << pomodoro->getLongBreakDuration() << " 分钟\n";
            int newDuration = getIntInput("请输入新的长休息时长 (1-60分钟): ");
            if (newDuration >= 1 && newDuration <= 60) {
                pomodoro->setLongBreakDuration(newDuration);
                displaySuccess("长休息时长已更新为 " + to_string(newDuration) + " 分钟");
            } else {
                displayError("无效的时长，请输入1-60之间的数字");
            }
            break;
        }
        case 0:
            return;
    }
    
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
