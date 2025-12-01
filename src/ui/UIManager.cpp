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
    delete statsAnalyzer;
    delete xpSystem;
    delete heatmap;
    delete projectManager;
    delete taskManager;
    delete pomodoro;
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
        if (i < filled) cout << "#";
        else cout << "-";
    }
    cout << COLOR_RESET << "] " << int(percentage * 100) << "%";
}

void UIManager::printEncouragement() {
    static const vector<string> quotes = {
        "Keep the streak alive!", 
        "Small steps, big progress.", 
        "You are unstoppable today!", 
        "Focus is the key to victory.",
        "Every task completed is a win!",
        "Building great habits!"
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
    printSeparator("-", 60);
    cout << COLOR_RESET;
    
    // 第一行：等级、称号和成就
    cout << " Lv." << level << " [" << COLOR_MAGENTA << title << COLOR_RESET << "]"
         << "    Achievements: " << achievements 
         << "    Streak: " << streak << " days\n";
    
    // 第二行：经验值进度条
    cout << " XP: ";
    printProgressBar(currentXP, nextLevelXP, 35, COLOR_GREEN);
    cout << " (" << currentXP << "/" << nextLevelXP << ")\n";
    
    cout << BOLD << COLOR_CYAN;
    printSeparator("-", 60);
    cout << COLOR_RESET;
    
    printEncouragement();
}

void UIManager::showTaskCompleteCelebration(int xpGained) {
    cout << "\n";
    for(int i = 0; i < 3; ++i) {
        cout << COLOR_YELLOW << "  *  Reward Unlocking...  *  " << COLOR_RESET << "\r";
        cout.flush();
        this_thread::sleep_for(chrono::milliseconds(150));
        cout << "                              \r"; 
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    
    cout << "\n  " << COLOR_GREEN << BOLD << "TASK COMPLETED! Awesome!" << COLOR_RESET << "\n";
    cout << "  " << COLOR_YELLOW << "+" << xpGained << " XP" << COLOR_RESET << "\n\n";
    
    this_thread::sleep_for(chrono::milliseconds(800)); 
}

// === 主界面 ===

void UIManager::showMainMenu() {
    clearScreen();
    
    cout << BOLD << COLOR_BLUE;
    cout << R"(
   ======================================================
   |    Smart Task Management System v2.0              |
   |         智能任务管理系统 - 游戏化版本             |
   ======================================================
)" << COLOR_RESET;
    
    displayHUD();
    
    vector<string> options = {
        "Task Management (任务管理)",
        "Project Management (项目管理)",
        "Pomodoro Timer (番茄钟)",
        "Statistics (统计分析)",
        "Gamification (游戏化功能)",
        "Settings (设置)"
    };
    
    printMenu(options);
}

void UIManager::run() {
    cout << COLOR_GREEN << "\n欢迎使用智能任务管理系统！\n" << COLOR_RESET;
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
        cout << COLOR_GREEN << "\n感谢使用！再见！\n\n" << COLOR_RESET;
        running = false;
    }
}

// === 任务管理界面 ===

void UIManager::showTaskMenu() {
    clearScreen();
    printHeader("Task Management (任务管理)");
    
    vector<string> options = {
        "Create Task (创建新任务)",
        "View All Tasks (查看所有任务)",
        "Update Task (更新任务)",
        "Delete Task (删除任务)",
        "Complete Task (完成任务)",
        "Assign to Project (分配任务到项目)"
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
    printHeader("Create New Task (创建新任务)");

    // 基本信息
    string name = getInput("Task Name (任务名称): ");
    if (name.empty()) {
        displayError("任务名称不能为空！");
        pause();
        return;
    }
    
    string desc = getInput("Description (任务描述，可选): ");
    
    // 选择优先级
    int priority = selectPriority();
    
    // 截止日期
    string due = getInput("Due Date (截止日期 YYYY-MM-DD，直接回车跳过): ");
    
    // 标签
    string tags = getInput("Tags (标签，用逗号分隔，直接回车跳过): ");
    
    // 预计番茄数
    cout << "\nEstimated Pomodoros (预计番茄数，每个25分钟，输入0跳过)\n";
    int estimated = getIntInput("   Pomodoros: ");
    
    // 提醒时间
    string reminder = getInput("Reminder Time (提醒时间 YYYY-MM-DD HH:MM，直接回车跳过): ");
    
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
        cout << "\n" << BOLD << "Task Summary (任务摘要)：" << COLOR_RESET << "\n";
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
    printHeader("Task List (任务列表)");
    
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
    
    cout << "\n" << COLOR_CYAN << "Statistics: " << COLOR_RESET 
         << completed << "/" << total << " completed ("
         << fixed << setprecision(0) << (total > 0 ? (completed * 100.0 / total) : 0) << "%)\n";
    
    printSeparator("-", 60);
    
    // 分类显示：未完成 -> 已完成
    cout << "\n" << BOLD << "Pending Tasks (进行中)：" << COLOR_RESET << "\n";
    bool hasPending = false;
    for (const auto& t : tasks) {
        if (!t.isCompleted()) {
            hasPending = true;
            cout << "  ";
            
            // 优先级图标
            int prio = t.getPriority();
            if (prio == 2) cout << COLOR_RED << "[H]";
            else if (prio == 1) cout << COLOR_YELLOW << "[M]";
            else cout << COLOR_GREEN << "[L]";
            cout << COLOR_RESET;
            
            cout << " " << t.getName();
            
            // 显示截止日期
            if (!t.getDueDate().empty()) {
                cout << COLOR_CYAN << " (Due: " << t.getDueDate() << ")" << COLOR_RESET;
            }
            
            // 显示番茄钟进度
            if (t.getEstimatedPomodoros() > 0) {
                cout << " P:" << t.getPomodoroCount() << "/" << t.getEstimatedPomodoros();
            }
            
            cout << "\n";
        }
    }
    if (!hasPending) {
        cout << "  " << COLOR_GREEN << "太棒了！没有待完成的任务！" << COLOR_RESET << "\n";
    }
    
    cout << "\n" << BOLD << "Completed Tasks (已完成)：" << COLOR_RESET << "\n";
    bool hasCompleted = false;
    for (const auto& t : tasks) {
        if (t.isCompleted()) {
            hasCompleted = true;
            cout << "  " << COLOR_GREEN << "[Done] " << t.getName() << COLOR_RESET << "\n";
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
    printHeader("Update Task (更新任务)");
    
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
    
    cout << "\n" << BOLD << "Current Task Info (当前任务信息)：" << COLOR_RESET << "\n";
    cout << "  Name: " << task.getName() << "\n";
    cout << "  Description: " << (task.getDescription().empty() ? "(none)" : task.getDescription()) << "\n";
    cout << "  Priority: " << task.getPriority() << "\n";
    cout << "  Status: " << (task.isCompleted() ? "Completed" : "Pending") << "\n";
    
    cout << "\n" << BOLD << "Select what to modify (选择要修改的内容)：" << COLOR_RESET << "\n";
    printSeparator("-", 40);
    cout << "  " << COLOR_YELLOW << "[1]" << COLOR_RESET << " Name (名称)\n";
    cout << "  " << COLOR_YELLOW << "[2]" << COLOR_RESET << " Description (描述)\n";
    cout << "  " << COLOR_YELLOW << "[3]" << COLOR_RESET << " Priority (优先级)\n";
    cout << "  " << COLOR_YELLOW << "[4]" << COLOR_RESET << " Due Date (截止日期)\n";
    cout << "  " << COLOR_YELLOW << "[5]" << COLOR_RESET << " Toggle Status (切换完成状态)\n";
    cout << "  " << COLOR_RED << "[0]" << COLOR_RESET << " Cancel (取消)\n";
    printSeparator("-", 40);
    
    int choice = getUserChoice(5);
    
    switch (choice) {
        case 1: {
            string newName = getInput("New Name (新名称): ");
            if (!newName.empty()) task.setName(newName);
            break;
        }
        case 2: {
            string newDesc = getInput("New Description (新描述): ");
            task.setDescription(newDesc);
            break;
        }
        case 3: {
            task.setPriority(selectPriority());
            break;
        }
        case 4: {
            string newDue = getInput("New Due Date (新截止日期 YYYY-MM-DD): ");
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
    printHeader("Delete Task (删除任务)");
    
    // 使用选择式输入
    int taskId = selectTaskByName();
    if (taskId < 0) {
        pause();
        return;
    }
    
    auto opt = taskManager->getTask(taskId);
    if (opt.has_value()) {
        cout << "\n" << COLOR_YELLOW << "即将删除任务: " << opt.value().getName() << COLOR_RESET << "\n";
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
    printHeader("Complete Task (完成任务)");
    
    auto tasks = taskManager->getAllTasks();
    
    // 筛选未完成的任务
    vector<Task> pendingTasks;
    for (const auto& t : tasks) {
        if (!t.isCompleted()) {
            pendingTasks.push_back(t);
        }
    }
    
    if (pendingTasks.empty()) {
        displayInfo("太棒了！没有待完成的任务！");
        pause();
        return;
    }
    
    cout << "\n" << BOLD << "Select task to complete (选择要完成的任务)：" << COLOR_RESET << "\n";
    printSeparator("-", 50);
    
    for (size_t i = 0; i < pendingTasks.size(); i++) {
        cout << "  " << COLOR_YELLOW << "[" << i + 1 << "]" << COLOR_RESET << " "
             << pendingTasks[i].getName();
        
        int prio = pendingTasks[i].getPriority();
        if (prio == 2) cout << COLOR_RED << " ***" << COLOR_RESET;
        else if (prio == 1) cout << COLOR_YELLOW << " **" << COLOR_RESET;
        else cout << COLOR_GREEN << " *" << COLOR_RESET;
        
        cout << "\n";
    }
    
    cout << "  " << COLOR_RED << "[0]" << COLOR_RESET << " Cancel (取消)\n";
    printSeparator("-", 50);
    
    int choice = getUserChoice(static_cast<int>(pendingTasks.size()));
    
    if (choice == 0) return;
    
    int taskId = pendingTasks[choice - 1].getId();
    int priority = pendingTasks[choice - 1].getPriority();
    
    if (taskManager->completeTask(taskId)) {
        int xpReward = xpSystem->getXPForTaskCompletion(priority);
        xpSystem->awardXP(xpReward, "Task completed: " + pendingTasks[choice - 1].getName());
        showTaskCompleteCelebration(xpReward);
    } else {
        displayError("操作失败！");
        pause();
    }
}

void UIManager::assignTaskToProject() {
    clearScreen();
    printHeader("Assign Task to Project (分配任务到项目)");
    
    cout << "\n" << BOLD << "Step 1: Select Task (选择任务)" << COLOR_RESET << "\n";
    int taskId = selectTaskByName();
    if (taskId < 0) {
        pause();
        return;
    }
    
    cout << "\n" << BOLD << "Step 2: Select Project (选择项目)" << COLOR_RESET << "\n";
    int projectId = selectProjectByName();
    
    if (projectId < 0) {
        displayInfo("已取消分配。");
        pause();
        return;
    }
    
    if (taskManager->assignTaskToProject(taskId, projectId)) {
        displaySuccess("任务已成功分配到项目！");
    } else {
        displayError("分配失败！");
    }
    
    pause();
}

// === 项目管理界面 ===

void UIManager::showProjectMenu() {
    clearScreen();
    printHeader("Project Management (项目管理)");
    
    vector<string> options = {
        "Create Project (创建新项目)",
        "View All Projects (查看所有项目)",
        "Project Details (查看项目详情)",
        "Update Project (更新项目)",
        "Delete Project (删除项目)"
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
    printHeader("Create New Project (创建新项目)");
    
    string name = getInput("Project Name (项目名称): ");
    if (name.empty()) {
        displayError("项目名称不能为空！");
        pause();
        return;
    }
    
    string desc = getInput("Description (项目描述): ");
    
    // 使用颜色选择替代手动输入
    string color = selectColor();
    
    Project project(name, desc, color);
    int id = projectManager->createProject(project);
    
    if (id > 0) {
        displaySuccess("项目创建成功！ID: " + to_string(id));
        
        cout << "\n" << BOLD << "Project Summary (项目摘要)：" << COLOR_RESET << "\n";
        cout << "  Name: " << name << "\n";
        cout << "  Description: " << (desc.empty() ? "(none)" : desc) << "\n";
        cout << "  Color: " << color << "\n";
    } else {
        displayError("创建失败！");
    }
    
    pause();
}

void UIManager::listProjects() {
    clearScreen();
    printHeader("Project List (项目列表)");
    
    vector<Project*> projects = projectManager->getAllProjects();
    
    if (projects.empty()) {
        displayInfo("暂无项目，赶快创建一个吧！");
        pause();
        return;
    }
    
    cout << "\n";
    printSeparator("-", 55);
    
    for (Project* p : projects) {
        cout << "\n  " << COLOR_BLUE << BOLD << p->getName() << COLOR_RESET << "\n";
        cout << "  " << "Description: " << (p->getDescription().empty() ? "(none)" : p->getDescription()) << "\n";
        
        // 进度条
        double prog = p->getProgress();
        cout << "  Progress: ";
        printProgressBar(static_cast<int>(prog * 100), 100, 20, COLOR_GREEN);
        cout << " (" << p->getCompletedTasks() << "/" << p->getTotalTasks() << " tasks)\n";
        
        printSeparator("-", 55);
    }
    
    pause();
}

void UIManager::viewProjectDetails() {
    clearScreen();
    printHeader("Project Details (项目详情)");
    
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
    cout << BOLD << "========================================\n";
    cout << "  Project: " << p->getName() << "\n";
    cout << "========================================" << COLOR_RESET << "\n\n";
    
    cout << "Description: " << (p->getDescription().empty() ? "(none)" : p->getDescription()) << "\n";
    cout << "Color: " << p->getColorLabel() << "\n";
    cout << "Target Date: " << (p->getTargetDate().empty() ? "(not set)" : p->getTargetDate()) << "\n";
    cout << "Created: " << p->getCreatedDate() << "\n";
    cout << "Status: " << (p->isArchived() ? "Archived" : "Active") << "\n";
    
    cout << "\n" << BOLD << "Task Statistics:" << COLOR_RESET << "\n";
    cout << "  Total Tasks: " << p->getTotalTasks() << "\n";
    cout << "  Completed: " << p->getCompletedTasks() << "\n";
    cout << "  Progress: ";
    printProgressBar(static_cast<int>(p->getProgress() * 100), 100, 25, COLOR_GREEN);
    cout << "\n";
    
    // 显示项目下的任务
    auto tasks = taskManager->getTasksByProject(projectId);
    if (!tasks.empty()) {
        cout << "\n" << BOLD << "Project Tasks:" << COLOR_RESET << "\n";
        for (const auto& t : tasks) {
            string status = t.isCompleted() ? COLOR_GREEN + "[Done]" : COLOR_YELLOW + "[Todo]";
            cout << "  " << status << COLOR_RESET << " " << t.getName() << "\n";
        }
    }
    
    pause();
}

void UIManager::updateProject() {
    clearScreen();
    printHeader("Update Project (更新项目)");
    
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
    
    cout << "\n" << BOLD << "Current Project Info (当前项目信息)：" << COLOR_RESET << "\n";
    cout << "  Name: " << p->getName() << "\n";
    cout << "  Description: " << p->getDescription() << "\n";
    cout << "  Color: " << p->getColorLabel() << "\n";
    
    cout << "\n" << BOLD << "Select what to modify (选择要修改的内容)：" << COLOR_RESET << "\n";
    printSeparator("-", 40);
    cout << "  " << COLOR_YELLOW << "[1]" << COLOR_RESET << " Name (名称)\n";
    cout << "  " << COLOR_YELLOW << "[2]" << COLOR_RESET << " Description (描述)\n";
    cout << "  " << COLOR_YELLOW << "[3]" << COLOR_RESET << " Color (颜色)\n";
    cout << "  " << COLOR_YELLOW << "[4]" << COLOR_RESET << " Target Date (目标日期)\n";
    cout << "  " << COLOR_YELLOW << "[5]" << COLOR_RESET << " Toggle Archive (归档/取消归档)\n";
    cout << "  " << COLOR_RED << "[0]" << COLOR_RESET << " Cancel (取消)\n";
    printSeparator("-", 40);
    
    int choice = getUserChoice(5);
    
    switch (choice) {
        case 1: {
            string newName = getInput("New Name (新名称): ");
            if (!newName.empty()) p->setName(newName);
            break;
        }
        case 2: {
            string newDesc = getInput("New Description (新描述): ");
            p->setDescription(newDesc);
            break;
        }
        case 3: {
            string newColor = selectColor();
            p->setColorLabel(newColor);
            break;
        }
        case 4: {
            string newDate = getInput("Target Date (目标日期 YYYY-MM-DD): ");
            p->setTargetDate(newDate);
            break;
        }
        case 5: {
            p->setArchived(!p->isArchived());
            displayInfo(p->isArchived() ? "项目已归档" : "项目已取消归档");
            break;
        }
        case 0:
            return;
    }
    
    if (projectManager->updateProject(*p)) {
        displaySuccess("项目更新成功！");
    } else {
        displayError("更新失败！");
    }
    
    pause();
}

void UIManager::deleteProject() {
    clearScreen();
    printHeader("Delete Project (删除项目)");
    
    // 使用选择式输入
    int projectId = selectProjectByName();
    if (projectId < 0) {
        pause();
        return;
    }
    
    Project* p = projectManager->getProject(projectId);
    if (p != nullptr) {
        cout << "\n" << COLOR_YELLOW << "即将删除项目: " << p->getName() << COLOR_RESET << "\n";
    }
    
    if (confirmAction("确定要删除这个项目吗？（项目下的任务不会被删除）")) {
        if (projectManager->deleteProject(projectId)) {
            displaySuccess("项目删除成功！");
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
    printHeader("Pomodoro Timer (番茄钟)");
    
    cout << "\n" << BOLD << "What is Pomodoro? (什么是番茄工作法？)" << COLOR_RESET << "\n";
    cout << "  Focus for 25 minutes, then take a 5-minute break.\n";
    cout << "  After 4 pomodoros, take a 15-30 minute break.\n";
    
    cout << "\n" << COLOR_CYAN << "Today's Pomodoros: " << COLOR_RESET 
         << statsAnalyzer->getPomodorosToday() << "\n";
    cout << COLOR_CYAN << "Total Pomodoros: " << COLOR_RESET 
         << statsAnalyzer->getTotalPomodoros() << "\n";
    
    vector<string> options = {
        "Start Work Session (开始工作 25分钟)",
        "Short Break (短休息 5分钟)",
        "Long Break (长休息 15分钟)"
    };
    
    printMenu(options);
    int choice = getUserChoice(3);
    
    switch (choice) {
        case 1: startPomodoroSession(); break;
        case 2: {
            displayInfo("开始短休息 (5分钟)...");
            cout << "  Tip: Stand up, stretch, drink some water!\n";
            pause();
            break;
        }
        case 3: {
            displayInfo("开始长休息 (15分钟)...");
            cout << "  Tip: Take a walk, rest your eyes!\n";
            pause();
            break;
        }
        case 0: return;
    }
}

void UIManager::startPomodoroSession() {
    clearScreen();
    printHeader("Pomodoro Work Session (番茄钟工作时间)");
    
    // 选择关联的任务（可选）
    cout << "\nWould you like to associate a task? (是否关联任务？)\n";
    cout << "  (Task's pomodoro count will +1 when completed)\n";
    int taskId = -1;
    if (confirmAction("Associate task? (关联任务？)")) {
        taskId = selectTaskByName();
    }
    
    cout << "\n" << COLOR_GREEN << BOLD << "Pomodoro Started! (番茄钟开始！)" << COLOR_RESET << "\n";
    cout << "  Focus Time: 25 minutes\n";
    cout << "  Stay focused, minimize distractions!\n\n";
    
    // 简化版：不实际等待25分钟，而是模拟完成
    cout << COLOR_YELLOW << "  (Demo mode: Press Enter to simulate completion)" << COLOR_RESET << "\n";
    pause();
    
    // 番茄钟完成
    cout << "\n" << COLOR_GREEN << BOLD << "Pomodoro Completed! (番茄钟完成！)" << COLOR_RESET << "\n";
    
    // 奖励XP
    int xpReward = xpSystem->getXPForPomodoro();
    xpSystem->awardXP(xpReward, "Pomodoro completed");
    
    // 如果关联了任务，增加任务的番茄数
    if (taskId > 0) {
        taskManager->addPomodoro(taskId);
        displaySuccess("Task pomodoro count +1");
    }
    
    cout << "\n  " << COLOR_CYAN << "Time for a break! (休息一下吧！)" << COLOR_RESET << "\n";
    pause();
}

// === 统计分析界面 ===

void UIManager::showStatisticsMenu() {
    clearScreen();
    printHeader("Statistics (统计分析)");
    
    vector<string> options = {
        "Summary (统计数据总览)",
        "Daily Report (每日报告)",
        "Weekly Report (每周报告)",
        "Monthly Report (每月报告)",
        "Task Heatmap (任务完成热力图)"
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
    printHeader("Statistics Summary (统计数据总览)");
    cout << statsAnalyzer->generateSummary();
    pause();
}

void UIManager::showDailyReport() {
    clearScreen();
    printHeader("Daily Report (每日报告)");
    cout << statsAnalyzer->generateDailyReport();
    pause();
}

void UIManager::showWeeklyReport() {
    clearScreen();
    printHeader("Weekly Report (每周报告)");
    cout << statsAnalyzer->generateWeeklyReport();
    pause();
}

void UIManager::showMonthlyReport() {
    clearScreen();
    printHeader("Monthly Report (每月报告)");
    cout << statsAnalyzer->generateMonthlyReport();
    pause();
}

void UIManager::showHeatmap() {
    clearScreen();
    printHeader("Task Heatmap (任务完成热力图)");
    cout << heatmap->generateHeatmap(90);
    pause();
}

// === 游戏化界面 ===

void UIManager::showGamificationMenu() {
    clearScreen();
    printHeader("Gamification (游戏化功能)");
    
    vector<string> options = {
        "XP & Level (经验值和等级)",
        "Achievements (成就系统)",
        "Challenges (挑战系统)"
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
    printHeader("XP & Level (经验值和等级)");
    cout << xpSystem->displayLevelInfo();
    
    cout << "\n" << BOLD << "How to earn XP (经验值获取方式)：" << COLOR_RESET << "\n";
    cout << "  - Complete task: " << COLOR_GREEN << "10-50 XP" << COLOR_RESET << " (by priority)\n";
    cout << "  - Complete pomodoro: " << COLOR_GREEN << "5 XP" << COLOR_RESET << "\n";
    cout << "  - Daily streak: " << COLOR_GREEN << "days x 10 XP" << COLOR_RESET << "\n";
    cout << "  - Unlock achievement: " << COLOR_GREEN << "varies" << COLOR_RESET << "\n";
    
    pause();
}

void UIManager::showAchievements() {
    clearScreen();
    printHeader("Achievements (成就系统)");
    
    int unlocked = statsAnalyzer->getAchievementsUnlocked();
    int total = 10; // 假设总共10个成就
    
    cout << "\n" << BOLD << "Achievement Progress: " << COLOR_RESET;
    printProgressBar(unlocked, total, 20, COLOR_YELLOW);
    cout << " (" << unlocked << "/" << total << ")\n\n";
    
    // 显示一些基本成就
    cout << BOLD << "Available Achievements (可用成就)：" << COLOR_RESET << "\n";
    printSeparator("-", 50);
    
    cout << (unlocked >= 1 ? COLOR_GREEN + "[Done]" : COLOR_YELLOW + "[Lock]") << COLOR_RESET
         << " First Step - Complete your first task\n";
    cout << (unlocked >= 2 ? COLOR_GREEN + "[Done]" : COLOR_YELLOW + "[Lock]") << COLOR_RESET
         << " Seven Day Streak - Complete tasks for 7 days\n";
    cout << (unlocked >= 3 ? COLOR_GREEN + "[Done]" : COLOR_YELLOW + "[Lock]") << COLOR_RESET
         << " Pomodoro Master - Complete 100 pomodoros\n";
    cout << (unlocked >= 4 ? COLOR_GREEN + "[Done]" : COLOR_YELLOW + "[Lock]") << COLOR_RESET
         << " Time Manager - Complete 5 tasks in one day\n";
    
    printSeparator("-", 50);
    displayInfo("Full achievement system coming soon...");
    
    pause();
}

void UIManager::showChallenges() {
    clearScreen();
    printHeader("Challenges (挑战系统)");
    
    int completed = statsAnalyzer->getChallengesCompleted();
    
    cout << "\n" << BOLD << "Completed Challenges: " << COLOR_RESET 
         << COLOR_GREEN << completed << COLOR_RESET << "\n\n";
    
    cout << BOLD << "Daily Challenges (每日挑战)：" << COLOR_RESET << "\n";
    printSeparator("-", 50);
    cout << "  [*] Daily Goal - Complete 3 tasks (Reward: 30XP)\n";
    cout << "  [*] Pomodoro Pro - Complete 4 pomodoros (Reward: 20XP)\n";
    
    cout << "\n" << BOLD << "Weekly Challenges (每周挑战)：" << COLOR_RESET << "\n";
    printSeparator("-", 50);
    cout << "  [*] Weekly Planner - Complete 15 tasks (Reward: 100XP)\n";
    cout << "  [*] Consistent Worker - 7-day streak (Reward: 70XP)\n";
    
    printSeparator("-", 50);
    displayInfo("Full challenge system coming soon...");
    
    pause();
}

// === 设置界面 ===

void UIManager::showSettingsMenu() {
    clearScreen();
    printHeader("Settings (系统设置)");
    
    vector<string> options = {
        "View Settings (查看当前设置)",
        "Pomodoro Duration (修改番茄钟时长)",
        "Notification Settings (修改通知设置)"
    };
    
    printMenu(options);
    int choice = getUserChoice(3);
    
    switch (choice) {
        case 1: viewSettings(); break;
        case 2: updateSettings(); break;
        case 3: {
            displayInfo("Notification settings coming soon...");
            pause();
            break;
        }
        case 0: return;
    }
}

void UIManager::viewSettings() {
    clearScreen();
    printHeader("Current Settings (当前设置)");
    
    cout << "\n" << BOLD << "Pomodoro Settings (番茄钟设置)：" << COLOR_RESET << "\n";
    cout << "  Work Duration: 25 minutes\n";
    cout << "  Short Break: 5 minutes\n";
    cout << "  Long Break: 15 minutes\n";
    cout << "  Long Break Interval: 4 pomodoros\n";
    
    cout << "\n" << BOLD << "Notification Settings (通知设置)：" << COLOR_RESET << "\n";
    cout << "  Sound: Enabled\n";
    cout << "  Desktop Notifications: Enabled\n";
    
    cout << "\n" << BOLD << "UI Settings (界面设置)：" << COLOR_RESET << "\n";
    cout << "  Theme: Default\n";
    cout << "  Language: Chinese\n";
    
    pause();
}

void UIManager::updateSettings() {
    clearScreen();
    printHeader("Modify Settings (修改设置)");
    
    cout << "\n" << BOLD << "Select setting to modify (选择要修改的设置)：" << COLOR_RESET << "\n";
    printSeparator("-", 40);
    cout << "  " << COLOR_YELLOW << "[1]" << COLOR_RESET << " Work Duration (番茄钟工作时长)\n";
    cout << "  " << COLOR_YELLOW << "[2]" << COLOR_RESET << " Short Break (短休息时长)\n";
    cout << "  " << COLOR_YELLOW << "[3]" << COLOR_RESET << " Long Break (长休息时长)\n";
    cout << "  " << COLOR_RED << "[0]" << COLOR_RESET << " Back (返回)\n";
    printSeparator("-", 40);
    
    int choice = getUserChoice(3);
    
    switch (choice) {
        case 1:
        case 2:
        case 3:
            displayInfo("Settings modification coming soon...");
            break;
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
    cout << COLOR_RED << "[Error] " << error << COLOR_RESET << "\n";
}

void UIManager::displaySuccess(const string& msg) {
    cout << COLOR_GREEN << "[Success] " << msg << COLOR_RESET << "\n";
}

void UIManager::displayWarning(const string& warning) {
    cout << COLOR_YELLOW << "[Warning] " << warning << COLOR_RESET << "\n";
}

void UIManager::displayInfo(const string& info) {
    cout << COLOR_CYAN << "[Info] " << info << COLOR_RESET << "\n";
}
