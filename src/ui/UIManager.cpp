/**
 * @file UIManager.cpp
 * @brief 智能任务管理系统 - UI管理器实现
 * 
 * 功能概述：
 * - 提供完整的用户界面交互
 * - 任务管理、项目管理、统计分析、游戏化功能
 * - 提醒管理（创建、查看、删除、重新安排）
 * - 成就管理（查看、检查解锁、统计）
 * - 番茄钟计时器
 * - 使用选择式输入提升用户体验
 * - 颜色选择和名称选择代替ID输入
 * 
 * UI设计原则：
 * - 有趣、创意、有亮点
 * - 兼具实用性和交互性
 * - 用户友好的选择式输入
 */

#include "ui/UIManager.h"
#include "database/DatabaseManager.h"
#include "statistics/StatisticsAnalyzer.h"
#include "gamification/XPSystem.h"
#include "HeatmapVisualizer/HeatmapVisualizer.h"
#include "project/ProjectManager.h"
#include "task/TaskManager.h"
#include "Pomodoro/pomodoro.h"
#include "reminder/ReminderSystem.h"
#include "database/DAO/ReminderDAO.h"
#include "achievement/AchievementManager.h"
#include "database/DAO/AchievementDAO.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <thread>
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>
#include <regex>

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
    
    // 初始化提醒系统
    auto reminderDAO = createReminderDAO("task_manager.db");
    reminderSystem = new ReminderSystem(std::move(reminderDAO));
    
    // 初始化成就系统
    auto achievementDAO = std::make_unique<AchievementDAO>("./data/");
    achievementMgr = new AchievementManager(std::move(achievementDAO), 1);
    
    cout << COLOR_GREEN << "✅ UI管理器初始化成功" << COLOR_RESET << endl;
}

UIManager::~UIManager() {
    if (statsAnalyzer) delete statsAnalyzer;
    if (xpSystem) delete xpSystem;
    if (heatmap) delete heatmap;
    if (projectManager) delete projectManager;
    if (taskManager) delete taskManager;
    if (pomodoro) delete pomodoro;
    if (reminderSystem) delete reminderSystem;
    if (achievementMgr) delete achievementMgr;
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

/**
 * @brief 验证日期格式是否为 YYYY-MM-DD
 * @param date 日期字符串
 * @return 是否有效
 */
bool isValidDateFormat(const string& date) {
    if (date.empty()) return true; // 空字符串允许跳过
    if (date.length() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;
    
    // 检查是否都是数字
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit(date[i])) return false;
    }
    
    // 检查月份和日期的合理性
    int year = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));
    
    if (year < 2020 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    
    // 简单的月份天数检查
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    // 闰年检查
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        daysInMonth[1] = 29;
    }
    if (day > daysInMonth[month - 1]) return false;
    
    return true;
}

/**
 * @brief 验证日期时间格式是否为 YYYY-MM-DD HH:MM:SS
 * @param datetime 日期时间字符串
 * @return 是否有效
 */
bool isValidDateTimeFormat(const string& datetime) {
    if (datetime.empty()) return true; // 空字符串允许跳过
    
    // 检查长度和格式 (YYYY-MM-DD HH:MM:SS)
    if (datetime.length() != 19) return false;
    if (datetime[4] != '-' || datetime[7] != '-') return false;
    if (datetime[10] != ' ') return false;
    if (datetime[13] != ':' || datetime[16] != ':') return false;
    
    // 检查日期部分
    string datePart = datetime.substr(0, 10);
    if (!isValidDateFormat(datePart)) return false;
    
    // 检查时间部分
    for (int i = 11; i < 19; i++) {
        if (i == 13 || i == 16) continue; // 跳过冒号位置
        if (!isdigit(datetime[i])) return false;
    }
    
    int hour = stoi(datetime.substr(11, 2));
    int minute = stoi(datetime.substr(14, 2));
    int second = stoi(datetime.substr(17, 2));
    
    if (hour < 0 || hour > 23) return false;
    if (minute < 0 || minute > 59) return false;
    if (second < 0 || second > 59) return false;
    
    return true;
}

/**
 * @brief 验证日期时间格式是否为 YYYY-MM-DD HH:MM
 * @param datetime 日期时间字符串 (简化格式，无秒)
 * @return 是否有效
 */
bool isValidDateTimeFormatShort(const string& datetime) {
    if (datetime.empty()) return true; // 空字符串允许跳过
    
    // 检查长度和格式 (YYYY-MM-DD HH:MM)
    if (datetime.length() != 16) return false;
    if (datetime[4] != '-' || datetime[7] != '-') return false;
    if (datetime[10] != ' ') return false;
    if (datetime[13] != ':') return false;
    
    // 检查日期部分
    string datePart = datetime.substr(0, 10);
    if (!isValidDateFormat(datePart)) return false;
    
    // 检查时间部分 (HH:MM)
    for (int i = 11; i < 16; i++) {
        if (i == 13) continue; // 跳过冒号位置
        if (!isdigit(datetime[i])) return false;
    }
    
    int hour = stoi(datetime.substr(11, 2));
    int minute = stoi(datetime.substr(14, 2));
    
    if (hour < 0 || hour > 23) return false;
    if (minute < 0 || minute > 59) return false;
    
    return true;
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
        displayWarning("暂无项目可选择。请先创建项目后再进行分配。");
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
        "⏰ 提醒管理 (Reminder Management)",
        "🍅 番茄钟 (Pomodoro Timer)",
        "📊 统计分析 (Statistics)",
        "🎮 游戏化功能 (Gamification)"
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
            case 3: showReminderMenu(); break;
            case 4: showPomodoroMenu(); break;
            case 5: showStatisticsMenu(); break;
            case 6: showGamificationMenu(); break;
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
    while (true) {
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
    
    // 截止日期 (带验证)
    string due;
    while (true) {
        due = getInput("📅 截止日期 (Due Date YYYY-MM-DD，直接回车跳过): ");
        if (due.empty() || isValidDateFormat(due)) break;
        displayError("日期格式错误！请使用 YYYY-MM-DD 格式（如 2025-12-31）");
    }
    
    // 标签
    string tags = getInput("🏷️  标签 (Tags，用逗号分隔，直接回车跳过): ");
    
    // 预计番茄数
    cout << "\n🍅 预计番茄数 (Estimated Pomodoros，每个25分钟，输入0跳过)\n";
    int estimated = getIntInput("   Pomodoros: ");
    
    // 提醒时间 (带验证)
    string reminder;
    while (true) {
        reminder = getInput("⏰ 提醒时间 (Reminder Time YYYY-MM-DD HH:MM，直接回车跳过): ");
        if (reminder.empty() || isValidDateTimeFormatShort(reminder)) break;
        displayError("时间格式错误！请使用 YYYY-MM-DD HH:MM 格式（如 2025-12-31 09:00）");
    }
    
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
            string newDue;
            while (true) {
                newDue = getInput("📅 新截止日期 (YYYY-MM-DD): ");
                if (newDue.empty() || isValidDateFormat(newDue)) break;
                displayError("日期格式错误！请使用 YYYY-MM-DD 格式（如 2025-12-31）");
            }
            if (!newDue.empty()) task.setDueDate(newDue);
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
    while (true) {
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
            string newDate;
            while (true) {
                newDate = getInput("📅 目标日期 (YYYY-MM-DD): ");
                if (newDate.empty() || isValidDateFormat(newDate)) break;
                displayError("日期格式错误！请使用 YYYY-MM-DD 格式（如 2025-12-31）");
            }
            if (!newDate.empty()) p->setTargetDate(newDate);
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
    while (true) {
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
    while (true) {
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
    while (true) {
        clearScreen();
        printHeader("🎮 游戏化功能 (Gamification)");
        
        vector<string> options = {
            "⭐ 经验值和等级 (XP & Level)",
            "🏆 成就系统 (Achievements)"
        };
        
        printMenu(options);
        int choice = getUserChoice(2);
        
        switch (choice) {
            case 1: showXPAndLevel(); break;
            case 2: showAchievementMenu(); break;
            case 0: return;
        }
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

/**
 * @brief 成就系统主菜单
 * 
 * 提供成就系统的完整功能入口
 */
void UIManager::showAchievementMenu() {
    while (true) {
        clearScreen();
        printHeader("🏆 成就系统 (Achievement System)");
        
        // 显示成就概览
        auto allAchievements = achievementMgr->getAchievementProgress(1);
        int unlockedCount = 0;
        for (const auto& prog : allAchievements) {
            if (prog.progressPercent >= 100.0) unlockedCount++;
        }
        
        cout << "\n" << COLOR_CYAN << "📊 成就概览: " << COLOR_RESET;
        cout << COLOR_GREEN << unlockedCount << COLOR_RESET << " / " 
             << allAchievements.size() << " 已解锁\n";
        
        vector<string> options = {
            "📋 查看所有成就 (All Achievements)",
            "✅ 已解锁成就 (Unlocked Achievements)",
            "📊 成就统计 (Statistics)",
            "🔄 检查成就解锁 (Check Achievements)"
        };
        
        printMenu(options);
        int choice = getUserChoice(4);
        
        switch (choice) {
            case 1: showAllAchievements(); break;
            case 2: showUnlockedAchievements(); break;
            case 3: showAchievementStatistics(); break;
            case 4: checkAchievements(); break;
            case 0: return;
        }
    }
}

/**
 * @brief 显示所有成就（旧接口，保持兼容）
 */
void UIManager::showAchievements() {
    showAchievementMenu();
}

/**
 * @brief 显示所有成就及其进度
 * 
 * 从AchievementManager读取所有成就定义并显示进度
 */
void UIManager::showAllAchievements() {
    clearScreen();
    printHeader("📋 所有成就 (All Achievements)");
    
    // 刷新成就数据
    achievementMgr->checkAllAchievements();
    auto allProgress = achievementMgr->getAchievementProgress(1);
    
    if (allProgress.empty()) {
        displayInfo("暂无成就数据，请先使用系统功能！");
        pause();
        return;
    }
    
    cout << "\n" << BOLD << "🏆 成就列表" << COLOR_RESET << "\n";
    printSeparator("═", 65);
    
    // 成就显示使用AchievementManager的displayAllAchievements
    // 但为了更好的UI效果，我们手动渲染
    
    // 从statsAnalyzer获取当前进度数据
    int totalTasks = statsAnalyzer->getTotalTasksCompleted();
    int streak = statsAnalyzer->getCurrentStreak();
    int totalPomodoros = statsAnalyzer->getTotalPomodoros();
    int todayTasks = statsAnalyzer->getTasksCompletedToday();
    
    // 成就1: 首次任务
    cout << "\n";
    bool ach1 = totalTasks >= 1;
    cout << "  " << (ach1 ? COLOR_GREEN + "✅" : COLOR_YELLOW + "🔒") << COLOR_RESET;
    cout << " " << BOLD << "🎯 首次任务" << COLOR_RESET << "\n";
    cout << "     完成第一个任务\n";
    cout << "     进度: ";
    printProgressBar(min(totalTasks, 1), 1, 20, ach1 ? COLOR_GREEN : COLOR_YELLOW);
    cout << " (" << min(totalTasks, 1) << "/1)\n";
    cout << "     奖励: " << COLOR_YELLOW << "+100 XP" << COLOR_RESET << "\n";
    
    // 成就2: 七日连胜
    cout << "\n";
    bool ach2 = streak >= 7;
    cout << "  " << (ach2 ? COLOR_GREEN + "✅" : COLOR_YELLOW + "🔒") << COLOR_RESET;
    cout << " " << BOLD << "🔥 七日连胜" << COLOR_RESET << "\n";
    cout << "     连续完成7天任务\n";
    cout << "     进度: ";
    printProgressBar(min(streak, 7), 7, 20, ach2 ? COLOR_GREEN : COLOR_YELLOW);
    cout << " (" << min(streak, 7) << "/7天)\n";
    cout << "     奖励: " << COLOR_YELLOW << "+300 XP" << COLOR_RESET << "\n";
    
    // 成就3: 时间管理达人
    cout << "\n";
    bool ach3 = todayTasks >= 10;
    cout << "  " << (ach3 ? COLOR_GREEN + "✅" : COLOR_YELLOW + "🔒") << COLOR_RESET;
    cout << " " << BOLD << "⏱️ 时间管理达人" << COLOR_RESET << "\n";
    cout << "     单日完成10个任务\n";
    cout << "     进度: ";
    printProgressBar(min(todayTasks, 10), 10, 20, ach3 ? COLOR_GREEN : COLOR_YELLOW);
    cout << " (今日: " << todayTasks << "/10)\n";
    cout << "     奖励: " << COLOR_YELLOW << "+200 XP" << COLOR_RESET << "\n";
    
    // 成就4: 番茄钟大师
    cout << "\n";
    bool ach4 = totalPomodoros >= 20;
    cout << "  " << (ach4 ? COLOR_GREEN + "✅" : COLOR_YELLOW + "🔒") << COLOR_RESET;
    cout << " " << BOLD << "🍅 番茄钟大师" << COLOR_RESET << "\n";
    cout << "     累计完成20个番茄钟\n";
    cout << "     进度: ";
    printProgressBar(min(totalPomodoros, 20), 20, 20, ach4 ? COLOR_GREEN : COLOR_YELLOW);
    cout << " (" << min(totalPomodoros, 20) << "/20)\n";
    cout << "     奖励: " << COLOR_YELLOW << "+250 XP" << COLOR_RESET << "\n";
    
    printSeparator("═", 65);
    
    // 统计
    int unlockedCount = (ach1 ? 1 : 0) + (ach2 ? 1 : 0) + (ach3 ? 1 : 0) + (ach4 ? 1 : 0);
    cout << "\n" << BOLD << "📈 完成进度: " << COLOR_RESET;
    printProgressBar(unlockedCount, 4, 25, COLOR_MAGENTA);
    cout << " (" << unlockedCount << "/4)\n";
    
    pause();
}

/**
 * @brief 显示已解锁的成就
 */
void UIManager::showUnlockedAchievements() {
    clearScreen();
    printHeader("✅ 已解锁成就 (Unlocked Achievements)");
    
    // 获取统计数据判断成就状态
    int totalTasks = statsAnalyzer->getTotalTasksCompleted();
    int streak = statsAnalyzer->getCurrentStreak();
    int totalPomodoros = statsAnalyzer->getTotalPomodoros();
    int todayTasks = statsAnalyzer->getTasksCompletedToday();
    
    bool ach1 = totalTasks >= 1;
    bool ach2 = streak >= 7;
    bool ach3 = todayTasks >= 10;
    bool ach4 = totalPomodoros >= 20;
    
    int unlockedCount = (ach1 ? 1 : 0) + (ach2 ? 1 : 0) + (ach3 ? 1 : 0) + (ach4 ? 1 : 0);
    
    if (unlockedCount == 0) {
        cout << "\n";
        cout << BOLD << COLOR_CYAN;
        cout << "   ╔══════════════════════════════════════════╗\n";
        cout << "   ║                                          ║\n";
        cout << "   ║     🎯 暂无已解锁成就                    ║\n";
        cout << "   ║                                          ║\n";
        cout << "   ║     完成任务、保持连续打卡               ║\n";
        cout << "   ║     来解锁你的第一个成就吧！             ║\n";
        cout << "   ║                                          ║\n";
        cout << "   ╚══════════════════════════════════════════╝\n";
        cout << COLOR_RESET;
        pause();
        return;
    }
    
    cout << "\n" << COLOR_GREEN << "🎉 恭喜！你已解锁 " << unlockedCount << " 个成就！" << COLOR_RESET << "\n\n";
    
    printSeparator("─", 55);
    
    if (ach1) {
        cout << "\n  " << COLOR_GREEN << "✅" << COLOR_RESET << " " << BOLD << "🎯 首次任务" << COLOR_RESET << "\n";
        cout << "     完成第一个任务\n";
        cout << "     获得: " << COLOR_YELLOW << "+100 XP" << COLOR_RESET << "\n";
    }
    
    if (ach2) {
        cout << "\n  " << COLOR_GREEN << "✅" << COLOR_RESET << " " << BOLD << "🔥 七日连胜" << COLOR_RESET << "\n";
        cout << "     连续完成7天任务\n";
        cout << "     获得: " << COLOR_YELLOW << "+300 XP" << COLOR_RESET << "\n";
    }
    
    if (ach3) {
        cout << "\n  " << COLOR_GREEN << "✅" << COLOR_RESET << " " << BOLD << "⏱️ 时间管理达人" << COLOR_RESET << "\n";
        cout << "     单日完成10个任务\n";
        cout << "     获得: " << COLOR_YELLOW << "+200 XP" << COLOR_RESET << "\n";
    }
    
    if (ach4) {
        cout << "\n  " << COLOR_GREEN << "✅" << COLOR_RESET << " " << BOLD << "🍅 番茄钟大师" << COLOR_RESET << "\n";
        cout << "     累计完成20个番茄钟\n";
        cout << "     获得: " << COLOR_YELLOW << "+250 XP" << COLOR_RESET << "\n";
    }
    
    printSeparator("─", 55);
    
    // 计算获得的总XP
    int totalXP = 0;
    if (ach1) totalXP += 100;
    if (ach2) totalXP += 300;
    if (ach3) totalXP += 200;
    if (ach4) totalXP += 250;
    
    cout << "\n" << BOLD << "💰 成就奖励总计: " << COLOR_YELLOW << totalXP << " XP" << COLOR_RESET << "\n";
    
    pause();
}

/**
 * @brief 显示成就统计信息
 */
void UIManager::showAchievementStatistics() {
    clearScreen();
    printHeader("📊 成就统计 (Achievement Statistics)");
    
    // 获取统计数据
    int totalTasks = statsAnalyzer->getTotalTasksCompleted();
    int streak = statsAnalyzer->getCurrentStreak();
    int totalPomodoros = statsAnalyzer->getTotalPomodoros();
    int todayTasks = statsAnalyzer->getTasksCompletedToday();
    
    bool ach1 = totalTasks >= 1;
    bool ach2 = streak >= 7;
    bool ach3 = todayTasks >= 10;
    bool ach4 = totalPomodoros >= 20;
    
    int unlockedCount = (ach1 ? 1 : 0) + (ach2 ? 1 : 0) + (ach3 ? 1 : 0) + (ach4 ? 1 : 0);
    int lockedCount = 4 - unlockedCount;
    double unlockRate = (unlockedCount * 100.0) / 4.0;
    
    cout << "\n";
    cout << BOLD << "╔═══════════════════════════════════════════════════╗\n";
    cout << "║           🏆 成就系统统计报告                     ║\n";
    cout << "╚═══════════════════════════════════════════════════╝" << COLOR_RESET << "\n\n";
    
    // 解锁进度
    cout << BOLD << "📈 解锁进度:" << COLOR_RESET << "\n";
    cout << "   ";
    printProgressBar(unlockedCount, 4, 30, COLOR_GREEN);
    cout << " " << fixed << setprecision(1) << unlockRate << "%\n\n";
    
    // 统计数据
    cout << BOLD << "📊 统计数据:" << COLOR_RESET << "\n";
    printSeparator("-", 45);
    cout << "  总成就数量:    " << COLOR_CYAN << "4" << COLOR_RESET << " 个\n";
    cout << "  已解锁成就:    " << COLOR_GREEN << unlockedCount << COLOR_RESET << " 个\n";
    cout << "  未解锁成就:    " << COLOR_YELLOW << lockedCount << COLOR_RESET << " 个\n";
    cout << "  解锁率:        " << COLOR_MAGENTA << fixed << setprecision(1) << unlockRate << "%" << COLOR_RESET << "\n";
    printSeparator("-", 45);
    
    // 当前进度
    cout << "\n" << BOLD << "🎯 当前进度数据:" << COLOR_RESET << "\n";
    printSeparator("-", 45);
    cout << "  累计完成任务:  " << COLOR_CYAN << totalTasks << COLOR_RESET << " 个\n";
    cout << "  连续打卡天数:  " << COLOR_CYAN << streak << COLOR_RESET << " 天\n";
    cout << "  今日完成任务:  " << COLOR_CYAN << todayTasks << COLOR_RESET << " 个\n";
    cout << "  累计番茄钟:    " << COLOR_CYAN << totalPomodoros << COLOR_RESET << " 个\n";
    printSeparator("-", 45);
    
    // XP奖励统计
    int totalXP = 0;
    int potentialXP = 100 + 300 + 200 + 250;  // 所有成就的总XP
    if (ach1) totalXP += 100;
    if (ach2) totalXP += 300;
    if (ach3) totalXP += 200;
    if (ach4) totalXP += 250;
    
    cout << "\n" << BOLD << "💰 XP奖励:" << COLOR_RESET << "\n";
    printSeparator("-", 45);
    cout << "  已获得XP:      " << COLOR_GREEN << totalXP << COLOR_RESET << " XP\n";
    cout << "  潜在XP:        " << COLOR_YELLOW << (potentialXP - totalXP) << COLOR_RESET << " XP\n";
    cout << "  总可用XP:      " << COLOR_MAGENTA << potentialXP << COLOR_RESET << " XP\n";
    printSeparator("-", 45);
    
    pause();
}

/**
 * @brief 检查并解锁成就
 * 
 * 调用AchievementManager检查所有成就条件
 */
void UIManager::checkAchievements() {
    clearScreen();
    printHeader("🔄 检查成就解锁 (Check Achievements)");
    
    cout << "\n" << COLOR_CYAN << "⏳ 正在检查成就解锁条件..." << COLOR_RESET << "\n\n";
    
    // 获取检查前的状态
    int totalTasks = statsAnalyzer->getTotalTasksCompleted();
    int streak = statsAnalyzer->getCurrentStreak();
    int totalPomodoros = statsAnalyzer->getTotalPomodoros();
    int todayTasks = statsAnalyzer->getTasksCompletedToday();
    
    // 显示检查动画
    cout << "  " << COLOR_YELLOW << "▶" << COLOR_RESET << " 检查任务成就... ";
    cout << (totalTasks >= 1 ? COLOR_GREEN + "✅ 已完成" : COLOR_YELLOW + "⏳ 进行中 (" + to_string(totalTasks) + "/1)") << COLOR_RESET << "\n";
    
    cout << "  " << COLOR_YELLOW << "▶" << COLOR_RESET << " 检查连续打卡成就... ";
    cout << (streak >= 7 ? COLOR_GREEN + "✅ 已完成" : COLOR_YELLOW + "⏳ 进行中 (" + to_string(streak) + "/7天)") << COLOR_RESET << "\n";
    
    cout << "  " << COLOR_YELLOW << "▶" << COLOR_RESET << " 检查时间管理成就... ";
    cout << (todayTasks >= 10 ? COLOR_GREEN + "✅ 已完成" : COLOR_YELLOW + "⏳ 进行中 (" + to_string(todayTasks) + "/10)") << COLOR_RESET << "\n";
    
    cout << "  " << COLOR_YELLOW << "▶" << COLOR_RESET << " 检查番茄钟成就... ";
    cout << (totalPomodoros >= 20 ? COLOR_GREEN + "✅ 已完成" : COLOR_YELLOW + "⏳ 进行中 (" + to_string(totalPomodoros) + "/20)") << COLOR_RESET << "\n";
    
    // 调用AchievementManager进行检查
    achievementMgr->checkAllAchievements();
    
    cout << "\n" << COLOR_GREEN << "✅ 成就检查完成！" << COLOR_RESET << "\n";
    
    // 显示提示
    cout << "\n" << BOLD << "💡 提示:" << COLOR_RESET << "\n";
    cout << "  继续完成任务、保持连续打卡、使用番茄钟\n";
    cout << "  来解锁更多成就获取XP奖励！\n";
    
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

// ============================================================================
// === 提醒系统 UI 界面 ===
// ============================================================================

/**
 * @brief 提醒管理主菜单
 * 
 * 提供提醒系统的完整功能入口，包括创建、查看、删除和重新安排提醒
 */
void UIManager::showReminderMenu() {
    while (true) {
        clearScreen();
        printHeader("⏰ 提醒管理 (Reminder Management)");
        
        // 显示今日待处理提醒数量
        auto todayReminders = reminderSystem->getDueRemindersForToday();
        if (!todayReminders.empty()) {
            cout << "\n" << COLOR_YELLOW << "📌 今日有 " << todayReminders.size() 
                 << " 个待处理提醒！" << COLOR_RESET << "\n";
        }
        
        vector<string> options = {
            "✨ 创建新提醒 (Create Reminder)",
            "📋 查看所有提醒 (View All Reminders)",
            "⏳ 查看待处理提醒 (Pending Reminders)",
            "📅 查看今日提醒 (Today's Reminders)",
            "🗑️  删除提醒 (Delete Reminder)",
            "🔄 重新安排提醒 (Reschedule Reminder)"
        };
        
        printMenu(options);
        int choice = getUserChoice(6);
        
        switch (choice) {
            case 1: createReminder(); break;
            case 2: listAllReminders(); break;
            case 3: listPendingReminders(); break;
            case 4: listTodayReminders(); break;
            case 5: deleteReminder(); break;
            case 6: rescheduleReminder(); break;
            case 0: return;
        }
    }
}

/**
 * @brief 选择重复规则
 * 
 * 提供友好的选择界面，让用户选择提醒的重复规则
 * @return 重复规则字符串 ("once", "daily", "weekly", "monthly")
 */
string UIManager::selectRecurrence() {
    cout << "\n" << BOLD << "请选择重复规则：" << COLOR_RESET << "\n";
    printSeparator("-", 40);
    cout << "  " << COLOR_YELLOW << "[1]" << COLOR_RESET << " 🔔 一次性 (Once)\n";
    cout << "  " << COLOR_YELLOW << "[2]" << COLOR_RESET << " 📆 每日 (Daily)\n";
    cout << "  " << COLOR_YELLOW << "[3]" << COLOR_RESET << " 📅 每周 (Weekly)\n";
    cout << "  " << COLOR_YELLOW << "[4]" << COLOR_RESET << " 🗓️  每月 (Monthly)\n";
    printSeparator("-", 40);
    
    int choice = getUserChoice(4);
    
    switch (choice) {
        case 1: return "once";
        case 2: return "daily";
        case 3: return "weekly";
        case 4: return "monthly";
        default: return "once";
    }
}

/**
 * @brief 创建新提醒
 * 
 * 引导用户创建新的提醒，支持关联任务和设置重复规则
 */
void UIManager::createReminder() {
    clearScreen();
    printHeader("✨ 创建新提醒 (Create New Reminder)");
    
    // 标题
    string title = getInput("📌 提醒标题 (Title): ");
    if (title.empty()) {
        displayError("提醒标题不能为空！");
        pause();
        return;
    }
    
    // 内容
    string message = getInput("📝 提醒内容 (Message): ");
    
    // 提醒时间 (带验证)
    string reminderTime;
    while (true) {
        cout << "\n" << COLOR_CYAN << "💡 提示：时间格式为 YYYY-MM-DD HH:MM:SS" << COLOR_RESET << "\n";
        cout << COLOR_CYAN << "   例如：2025-12-31 09:00:00" << COLOR_RESET << "\n";
        reminderTime = getInput("⏰ 提醒时间: ");
        
        if (reminderTime.empty()) {
            displayError("提醒时间不能为空！");
            continue;
        }
        
        if (isValidDateTimeFormat(reminderTime)) {
            break;
        }
        displayError("时间格式错误！请使用 YYYY-MM-DD HH:MM:SS 格式");
    }
    
    // 选择重复规则
    string recurrence = selectRecurrence();
    
    // 是否关联任务
    int taskId = 0;
    if (confirmAction("是否关联到任务？")) {
        int selectedTaskId = selectTaskByName();
        if (selectedTaskId > 0) {
            taskId = selectedTaskId;
        }
    }
    
    // 创建提醒
    reminderSystem->addReminder(title, message, reminderTime, recurrence, taskId);
    
    // 显示创建摘要
    cout << "\n" << BOLD << "📋 提醒摘要：" << COLOR_RESET << "\n";
    printSeparator("-", 45);
    cout << "  📌 标题: " << title << "\n";
    cout << "  📝 内容: " << (message.empty() ? "(无)" : message) << "\n";
    cout << "  ⏰ 时间: " << reminderTime << "\n";
    cout << "  🔄 重复: ";
    if (recurrence == "once") cout << "一次性";
    else if (recurrence == "daily") cout << "每日";
    else if (recurrence == "weekly") cout << "每周";
    else if (recurrence == "monthly") cout << "每月";
    cout << "\n";
    if (taskId > 0) {
        cout << "  📎 关联任务ID: " << taskId << "\n";
    }
    printSeparator("-", 45);
    
    pause();
}

/**
 * @brief 查看所有提醒
 * 
 * 显示系统中的所有提醒，包括已触发和待处理的
 */
void UIManager::listAllReminders() {
    clearScreen();
    printHeader("📋 所有提醒 (All Reminders)");
    
    auto reminders = reminderSystem->getActiveReminders();
    
    if (reminders.empty()) {
        displayInfo("🎉 暂无提醒，生活轻松无负担！");
        cout << "\n" << COLOR_CYAN << "💡 提示：返回菜单创建新的提醒吧！" << COLOR_RESET << "\n";
        pause();
        return;
    }
    
    cout << "\n" << COLOR_CYAN << "📊 共 " << reminders.size() << " 个提醒" << COLOR_RESET << "\n\n";
    
    printSeparator("─", 60);
    
    for (const auto& reminder : reminders) {
        // 状态图标
        string statusIcon = reminder.triggered ? COLOR_GREEN + "✅" : COLOR_YELLOW + "⏳";
        
        cout << statusIcon << COLOR_RESET << " ";
        cout << BOLD << "[ID:" << reminder.id << "] " << COLOR_RESET;
        cout << reminder.title << "\n";
        
        cout << "   📝 " << (reminder.message.empty() ? "(无内容)" : reminder.message) << "\n";
        cout << "   ⏰ " << reminder.trigger_time;
        
        // 重复规则显示
        cout << "  🔄 ";
        if (reminder.recurrence == "once") cout << "一次性";
        else if (reminder.recurrence == "daily") cout << COLOR_CYAN << "每日" << COLOR_RESET;
        else if (reminder.recurrence == "weekly") cout << COLOR_MAGENTA << "每周" << COLOR_RESET;
        else if (reminder.recurrence == "monthly") cout << COLOR_BLUE << "每月" << COLOR_RESET;
        
        // 关联任务
        if (reminder.task_id > 0) {
            cout << "  📎 任务#" << reminder.task_id;
        }
        
        cout << "\n";
        printSeparator("─", 60);
    }
    
    pause();
}

/**
 * @brief 查看待处理提醒
 * 
 * 只显示尚未触发的活跃提醒
 */
void UIManager::listPendingReminders() {
    clearScreen();
    printHeader("⏳ 待处理提醒 (Pending Reminders)");
    
    auto reminders = reminderSystem->getActiveReminders();
    
    // 筛选未触发的提醒
    vector<Reminder> pendingReminders;
    for (const auto& r : reminders) {
        if (!r.triggered && r.enabled) {
            pendingReminders.push_back(r);
        }
    }
    
    if (pendingReminders.empty()) {
        displayInfo("🎉 没有待处理的提醒！");
        cout << "\n" << COLOR_GREEN << "  太棒了，一切尽在掌控！" << COLOR_RESET << "\n";
        pause();
        return;
    }
    
    cout << "\n" << COLOR_YELLOW << "📌 " << pendingReminders.size() 
         << " 个提醒等待处理" << COLOR_RESET << "\n\n";
    
    printSeparator("─", 55);
    
    for (size_t i = 0; i < pendingReminders.size(); i++) {
        const auto& reminder = pendingReminders[i];
        
        cout << "  " << COLOR_YELLOW << "[" << (i + 1) << "]" << COLOR_RESET << " ";
        cout << "⏰ " << reminder.trigger_time << "\n";
        cout << "      📌 " << BOLD << reminder.title << COLOR_RESET << "\n";
        
        if (!reminder.message.empty()) {
            cout << "      📝 " << reminder.message << "\n";
        }
        
        // 显示重复类型
        cout << "      🔄 ";
        if (reminder.recurrence == "once") cout << "一次性";
        else if (reminder.recurrence == "daily") cout << COLOR_CYAN << "每日重复" << COLOR_RESET;
        else if (reminder.recurrence == "weekly") cout << COLOR_MAGENTA << "每周重复" << COLOR_RESET;
        else if (reminder.recurrence == "monthly") cout << COLOR_BLUE << "每月重复" << COLOR_RESET;
        cout << "\n";
        
        printSeparator("─", 55);
    }
    
    pause();
}

/**
 * @brief 查看今日提醒
 * 
 * 显示今天需要处理的所有提醒
 */
void UIManager::listTodayReminders() {
    clearScreen();
    printHeader("📅 今日提醒 (Today's Reminders)");
    
    auto todayReminders = reminderSystem->getDueRemindersForToday();
    
    if (todayReminders.empty()) {
        displayInfo("🌟 今天没有提醒，轻松愉快的一天！");
        cout << "\n";
        cout << BOLD << COLOR_CYAN;
        cout << "   ╔══════════════════════════════════╗\n";
        cout << "   ║     今天可以专注于重要的事情     ║\n";
        cout << "   ╚══════════════════════════════════╝\n";
        cout << COLOR_RESET;
        pause();
        return;
    }
    
    cout << "\n" << BOLD << "📅 今日提醒清单" << COLOR_RESET << "\n";
    cout << COLOR_YELLOW << "   共 " << todayReminders.size() << " 个提醒需要处理" << COLOR_RESET << "\n\n";
    
    printSeparator("═", 50);
    
    for (size_t i = 0; i < todayReminders.size(); i++) {
        const auto& reminder = todayReminders[i];
        
        // 时间提取 (只显示时间部分)
        string timeOnly = reminder.trigger_time;
        if (timeOnly.length() >= 19) {
            timeOnly = timeOnly.substr(11, 8);  // HH:MM:SS
        }
        
        cout << "\n  " << COLOR_CYAN << "⏰ " << timeOnly << COLOR_RESET;
        cout << "  " << BOLD << reminder.title << COLOR_RESET << "\n";
        
        if (!reminder.message.empty()) {
            cout << "     📝 " << reminder.message << "\n";
        }
        
        // 任务关联
        if (reminder.task_id > 0) {
            cout << "     📎 关联任务 #" << reminder.task_id << "\n";
        }
    }
    
    printSeparator("═", 50);
    
    cout << "\n" << COLOR_GREEN << "💪 加油，完成今日目标！" << COLOR_RESET << "\n";
    
    pause();
}

/**
 * @brief 删除提醒
 * 
 * 选择并删除一个提醒
 */
void UIManager::deleteReminder() {
    clearScreen();
    printHeader("🗑️  删除提醒 (Delete Reminder)");
    
    auto reminders = reminderSystem->getActiveReminders();
    
    if (reminders.empty()) {
        displayInfo("暂无提醒可删除");
        pause();
        return;
    }
    
    cout << "\n" << BOLD << "请选择要删除的提醒：" << COLOR_RESET << "\n";
    printSeparator("-", 50);
    
    for (size_t i = 0; i < reminders.size(); i++) {
        const auto& r = reminders[i];
        cout << "  " << COLOR_YELLOW << "[" << (i + 1) << "]" << COLOR_RESET << " ";
        cout << r.title << " (" << r.trigger_time << ")\n";
    }
    
    cout << "  " << COLOR_RED << "[0]" << COLOR_RESET << " 取消\n";
    printSeparator("-", 50);
    
    int choice = getUserChoice(static_cast<int>(reminders.size()));
    
    if (choice == 0) {
        displayInfo("已取消删除操作");
        pause();
        return;
    }
    
    const auto& selectedReminder = reminders[choice - 1];
    
    cout << "\n" << COLOR_YELLOW << "⚠️  即将删除提醒: " << selectedReminder.title << COLOR_RESET << "\n";
    
    if (confirmAction("确定要删除这个提醒吗？")) {
        if (reminderSystem->deleteReminder(selectedReminder.id)) {
            displaySuccess("🎉 提醒已删除");
        } else {
            displayError("删除失败，请重试");
        }
    } else {
        displayInfo("已取消删除操作");
    }
    
    pause();
}

/**
 * @brief 重新安排提醒时间
 * 
 * 选择一个提醒并重新设置其触发时间
 */
void UIManager::rescheduleReminder() {
    clearScreen();
    printHeader("🔄 重新安排提醒 (Reschedule Reminder)");
    
    auto reminders = reminderSystem->getActiveReminders();
    
    // 只显示未触发的提醒
    vector<Reminder> pendingReminders;
    for (const auto& r : reminders) {
        if (!r.triggered && r.enabled) {
            pendingReminders.push_back(r);
        }
    }
    
    if (pendingReminders.empty()) {
        displayInfo("暂无可重新安排的提醒");
        pause();
        return;
    }
    
    cout << "\n" << BOLD << "请选择要重新安排的提醒：" << COLOR_RESET << "\n";
    printSeparator("-", 55);
    
    for (size_t i = 0; i < pendingReminders.size(); i++) {
        const auto& r = pendingReminders[i];
        cout << "  " << COLOR_YELLOW << "[" << (i + 1) << "]" << COLOR_RESET << " ";
        cout << r.title << "\n";
        cout << "      当前时间: " << COLOR_CYAN << r.trigger_time << COLOR_RESET << "\n";
    }
    
    cout << "  " << COLOR_RED << "[0]" << COLOR_RESET << " 取消\n";
    printSeparator("-", 55);
    
    int choice = getUserChoice(static_cast<int>(pendingReminders.size()));
    
    if (choice == 0) {
        displayInfo("已取消操作");
        pause();
        return;
    }
    
    const auto& selectedReminder = pendingReminders[choice - 1];
    
    cout << "\n📌 当前提醒: " << BOLD << selectedReminder.title << COLOR_RESET << "\n";
    cout << "⏰ 当前时间: " << selectedReminder.trigger_time << "\n\n";
    
    // 输入新时间
    string newTime;
    while (true) {
        cout << COLOR_CYAN << "💡 时间格式: YYYY-MM-DD HH:MM:SS" << COLOR_RESET << "\n";
        newTime = getInput("📅 新的提醒时间: ");
        
        if (newTime.empty()) {
            displayError("时间不能为空！");
            continue;
        }
        
        if (isValidDateTimeFormat(newTime)) {
            break;
        }
        displayError("时间格式错误！请使用 YYYY-MM-DD HH:MM:SS 格式");
    }
    
    if (reminderSystem->rescheduleReminder(selectedReminder.id, newTime)) {
        displaySuccess("🎉 提醒时间已更新！");
        cout << "\n  📌 " << selectedReminder.title << "\n";
        cout << "  ⏰ 新时间: " << COLOR_GREEN << newTime << COLOR_RESET << "\n";
    } else {
        displayError("更新失败，请重试");
    }
    
    pause();
}
