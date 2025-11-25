#include "ui/UIManager.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <ctime>

using namespace std;

// ANSI颜色定义 - 标准色
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

// 亮色
const string UIManager::COLOR_BRIGHT_RED = "\033[91m";
const string UIManager::COLOR_BRIGHT_GREEN = "\033[92m";
const string UIManager::COLOR_BRIGHT_YELLOW = "\033[93m";
const string UIManager::COLOR_BRIGHT_BLUE = "\033[94m";
const string UIManager::COLOR_BRIGHT_MAGENTA = "\033[95m";
const string UIManager::COLOR_BRIGHT_CYAN = "\033[96m";
const string UIManager::COLOR_BRIGHT_WHITE = "\033[97m";

// 背景色
const string UIManager::BG_BLUE = "\033[44m";
const string UIManager::BG_GREEN = "\033[42m";
const string UIManager::BG_CYAN = "\033[46m";

// 常用消息常量
static const string MSG_MODULE_IN_DEVELOPMENT = "任务管理模块正在开发中...";
static const string MSG_SETTINGS_IN_DEVELOPMENT = "设置修改功能开发中...";
static const string DEFAULT_PROJECT_COLOR = "#3498db";

UIManager::UIManager() {
    running = true;
    
    // 创建各模块实例
    statsAnalyzer = new StatisticsAnalyzer();
    xpSystem = new XPSystem();
    heatmap = new HeatmapVisualizer();
    projectManager = new ProjectManager();
    
    cout << COLOR_BRIGHT_GREEN << "✅ UI管理器初始化成功" << COLOR_RESET << endl;
}

UIManager::~UIManager() {
    delete statsAnalyzer;
    delete xpSystem;
    delete heatmap;
    delete projectManager;
}

// === UI辅助方法 ===

void UIManager::clearScreen() {
    // ANSI转义序列清屏
    cout << "\033[2J\033[H";
}

void UIManager::printHeader(const string& title) {
    cout << "\n";
    cout << BOLD << COLOR_BRIGHT_CYAN;
    cout << "╔═══════════════════════════════════════════════════════╗\n";
    cout << "║  " << setw(53) << left << title << "║\n";
    cout << "╚═══════════════════════════════════════════════════════╝\n";
    cout << COLOR_RESET << "\n";
}

void UIManager::printSeparator(char symbol, int length) {
    for (int i = 0; i < length; i++) {
        cout << symbol;
    }
    cout << "\n";
}

void UIManager::printUTF8Separator(const string& symbol, int count) {
    for (int i = 0; i < count; i++) {
        cout << symbol;
    }
    cout << "\n";
}

void UIManager::printFancyBox(const string& content, const string& color) {
    string c = color.empty() ? COLOR_BRIGHT_CYAN : color;
    cout << c << "┌─────────────────────────────────────────────────────┐\n";
    cout << "│  " << setw(51) << left << content << "│\n";
    cout << "└─────────────────────────────────────────────────────┘" << COLOR_RESET << "\n";
}

void UIManager::printProgressBar(double progress, int width, const string& label) {
    int filled = static_cast<int>(progress * width);
    int empty = width - filled;
    
    cout << COLOR_BRIGHT_WHITE << label;
    cout << " [" << COLOR_BRIGHT_GREEN;
    for (int i = 0; i < filled; i++) cout << "█";
    cout << COLOR_WHITE;
    for (int i = 0; i < empty; i++) cout << "░";
    cout << COLOR_BRIGHT_WHITE << "] " << fixed << setprecision(1) << (progress * 100) << "%" << COLOR_RESET << "\n";
}

void UIManager::printMenu(const vector<string>& options) {
    cout << "\n";
    cout << COLOR_WHITE << "  ┌─────────────────────────────────────────────────┐\n";
    for (size_t i = 0; i < options.size(); i++) {
        cout << "  │  " << COLOR_BRIGHT_YELLOW << BOLD << i + 1 << COLOR_RESET 
             << COLOR_WHITE << " │ " << COLOR_BRIGHT_WHITE << setw(40) << left << options[i] 
             << COLOR_WHITE << "│\n";
    }
    cout << "  │  " << COLOR_BRIGHT_RED << BOLD << "0" << COLOR_RESET 
         << COLOR_WHITE << " │ " << COLOR_BRIGHT_WHITE << setw(40) << left << "🚪 返回/退出" 
         << COLOR_WHITE << "│\n";
    cout << "  └─────────────────────────────────────────────────┘" << COLOR_RESET << "\n";
    cout << "\n";
}

int UIManager::getUserChoice(int maxChoice) {
    int choice;
    cout << COLOR_BRIGHT_GREEN << "  ➤ 请选择 (0-" << maxChoice << "): " << COLOR_RESET;
    
    while (!(cin >> choice) || choice < 0 || choice > maxChoice) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << COLOR_BRIGHT_RED << "  ✗ 无效输入！请重新选择 (0-" << maxChoice << "): " << COLOR_RESET;
    }
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return choice;
}

string UIManager::getInput(const string& prompt) {
    string input;
    cout << COLOR_BRIGHT_GREEN << "  ➤ " << prompt << COLOR_RESET;
    getline(cin, input);
    return input;
}

int UIManager::getIntInput(const string& prompt) {
    int value;
    cout << COLOR_BRIGHT_GREEN << "  ➤ " << prompt << COLOR_RESET;
    
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << COLOR_BRIGHT_RED << "  ✗ 无效输入！请输入数字: " << COLOR_RESET;
    }
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

void UIManager::pause() {
    cout << "\n" << COLOR_BRIGHT_YELLOW << "  ⏎ 按Enter键继续..." << COLOR_RESET;
    cin.get();
}

bool UIManager::confirmAction(const string& prompt) {
    cout << COLOR_BRIGHT_YELLOW << "  ❓ " << prompt << " (y/n): " << COLOR_RESET;
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
    double progress = (nextLevelXP > 0) ? static_cast<double>(totalXP) / nextLevelXP : 0.0;
    
    // 用户状态栏 - 带fancy边框
    cout << COLOR_BRIGHT_CYAN << "  ╭───────────────────────────────────────────────────╮\n";
    cout << "  │  " << COLOR_BRIGHT_MAGENTA << "👤 " << COLOR_BRIGHT_WHITE << BOLD 
         << "Lv." << level << COLOR_RESET << COLOR_BRIGHT_CYAN 
         << " 【" << COLOR_BRIGHT_YELLOW << title << COLOR_BRIGHT_CYAN << "】";
    
    // 填充空格对齐
    cout << setw(30) << right << "";
    cout << "│\n";
    
    // XP进度条
    cout << "  │  " << COLOR_BRIGHT_YELLOW << "⚡ XP: " << COLOR_RESET;
    int barWidth = 20;
    int filled = static_cast<int>(progress * barWidth);
    cout << COLOR_BRIGHT_GREEN;
    for (int i = 0; i < filled; i++) cout << "█";
    cout << COLOR_WHITE;
    for (int i = filled; i < barWidth; i++) cout << "░";
    cout << COLOR_BRIGHT_WHITE << " " << totalXP << "/" << nextLevelXP;
    cout << setw(6) << right << "";
    cout << COLOR_BRIGHT_CYAN << "│\n";
    
    // 成就信息
    cout << "  │  " << COLOR_BRIGHT_GREEN << "🏆 成就: " << COLOR_BRIGHT_WHITE << achievements 
         << COLOR_RESET << " 个已解锁";
    cout << setw(23) << right << "";
    cout << COLOR_BRIGHT_CYAN << "│\n";
    
    cout << "  ╰───────────────────────────────────────────────────╯" << COLOR_RESET << "\n\n";
}

// === 主界面 ===

void UIManager::showMainMenu() {
    clearScreen();
    
    // 获取当前时间用于显示问候语
    time_t now = time(0);
    tm* ltm = localtime(&now);
    int hour = ltm->tm_hour;
    string greeting;
    string timeEmoji;
    
    if (hour >= 5 && hour < 12) {
        greeting = "早上好！新的一天，新的开始！";
        timeEmoji = "🌅";
    } else if (hour >= 12 && hour < 14) {
        greeting = "中午好！记得休息一下！";
        timeEmoji = "☀️";
    } else if (hour >= 14 && hour < 18) {
        greeting = "下午好！保持高效工作！";
        timeEmoji = "🌤️";
    } else if (hour >= 18 && hour < 22) {
        greeting = "晚上好！今天的任务完成了吗？";
        timeEmoji = "🌙";
    } else {
        greeting = "夜深了！注意休息哦！";
        timeEmoji = "🌃";
    }
    
    // Fancy ASCII Art Banner with gradient effect
    cout << "\n";
    cout << COLOR_BRIGHT_BLUE << BOLD;
    cout << R"(   ╔═══════════════════════════════════════════════════════════╗)" << "\n";
    cout << R"(   ║                                                           ║)" << "\n";
    cout << COLOR_BRIGHT_CYAN;
    cout << R"(   ║   )" << COLOR_BRIGHT_GREEN << "███████╗" << COLOR_BRIGHT_CYAN << "███╗   ███╗" << COLOR_BRIGHT_YELLOW << " █████╗ " << COLOR_BRIGHT_MAGENTA << "██████╗ " << COLOR_BRIGHT_RED << "████████╗" << COLOR_BRIGHT_BLUE << R"(          ║)" << "\n";
    cout << R"(   ║   )" << COLOR_BRIGHT_GREEN << "██╔════╝" << COLOR_BRIGHT_CYAN << "████╗ ████║" << COLOR_BRIGHT_YELLOW << "██╔══██╗" << COLOR_BRIGHT_MAGENTA << "██╔══██╗" << COLOR_BRIGHT_RED << "╚══██╔══╝" << COLOR_BRIGHT_BLUE << R"(          ║)" << "\n";
    cout << R"(   ║   )" << COLOR_BRIGHT_GREEN << "███████╗" << COLOR_BRIGHT_CYAN << "██╔████╔██║" << COLOR_BRIGHT_YELLOW << "███████║" << COLOR_BRIGHT_MAGENTA << "██████╔╝" << COLOR_BRIGHT_RED << "   ██║   " << COLOR_BRIGHT_BLUE << R"(          ║)" << "\n";
    cout << R"(   ║   )" << COLOR_BRIGHT_GREEN << "╚════██║" << COLOR_BRIGHT_CYAN << "██║╚██╔╝██║" << COLOR_BRIGHT_YELLOW << "██╔══██║" << COLOR_BRIGHT_MAGENTA << "██╔══██╗" << COLOR_BRIGHT_RED << "   ██║   " << COLOR_BRIGHT_BLUE << R"(          ║)" << "\n";
    cout << R"(   ║   )" << COLOR_BRIGHT_GREEN << "███████║" << COLOR_BRIGHT_CYAN << "██║ ╚═╝ ██║" << COLOR_BRIGHT_YELLOW << "██║  ██║" << COLOR_BRIGHT_MAGENTA << "██║  ██║" << COLOR_BRIGHT_RED << "   ██║   " << COLOR_BRIGHT_BLUE << R"(          ║)" << "\n";
    cout << R"(   ║   )" << COLOR_BRIGHT_GREEN << "╚══════╝" << COLOR_BRIGHT_CYAN << "╚═╝     ╚═╝" << COLOR_BRIGHT_YELLOW << "╚═╝  ╚═╝" << COLOR_BRIGHT_MAGENTA << "╚═╝  ╚═╝" << COLOR_BRIGHT_RED << "   ╚═╝   " << COLOR_BRIGHT_BLUE << R"(          ║)" << "\n";
    cout << COLOR_BRIGHT_BLUE;
    cout << R"(   ║                                                           ║)" << "\n";
    cout << R"(   ║)" << COLOR_BRIGHT_WHITE << "    🎮 Smart Task Management System v1.0 🎮            " << COLOR_BRIGHT_BLUE << R"(║)" << "\n";
    cout << R"(   ║)" << COLOR_BRIGHT_CYAN << "         ✨ 让任务管理变得有趣！✨                     " << COLOR_BRIGHT_BLUE << R"(║)" << "\n";
    cout << R"(   ║                                                           ║)" << "\n";
    cout << R"(   ╚═══════════════════════════════════════════════════════════╝)" << COLOR_RESET << "\n\n";
    
    // 问候语
    cout << "  " << timeEmoji << " " << COLOR_BRIGHT_WHITE << greeting << COLOR_RESET << "\n\n";
    
    displayUserStatusBar();
    
    vector<string> options = {
        "📋 任务管理 - 创建和管理您的任务",
        "📁 项目管理 - 组织您的项目",
        "📊 统计分析 - 查看您的效率数据",
        "🎮 游戏化功能 - XP、成就与挑战",
        "⚙️  系统设置 - 个性化您的体验"
    };
    
    printMenu(options);
}

void UIManager::run() {
    // Welcome animation effect
    clearScreen();
    cout << "\n\n";
    cout << COLOR_BRIGHT_CYAN << "    ╭────────────────────────────────────────────╮\n";
    cout << "    │  " << COLOR_BRIGHT_WHITE << "🌟 欢迎使用智能任务管理系统！🌟         " << COLOR_BRIGHT_CYAN << "│\n";
    cout << "    │  " << COLOR_BRIGHT_YELLOW << "   让每一天都充满成就感！               " << COLOR_BRIGHT_CYAN << "│\n";
    cout << "    ╰────────────────────────────────────────────╯" << COLOR_RESET << "\n\n";
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
        cout << "\n\n";
        cout << COLOR_BRIGHT_CYAN << "    ╭────────────────────────────────────────────╮\n";
        cout << "    │                                            │\n";
        cout << "    │     " << COLOR_BRIGHT_YELLOW << "👋 感谢使用！期待下次再见！" << COLOR_BRIGHT_CYAN << "          │\n";
        cout << "    │                                            │\n";
        cout << "    │     " << COLOR_BRIGHT_GREEN << "🌟 愿你的每一天都充满效率！" << COLOR_BRIGHT_CYAN << "        │\n";
        cout << "    │                                            │\n";
        cout << "    ╰────────────────────────────────────────────╯" << COLOR_RESET << "\n\n";
        running = false;
    }
}

// === 任务管理界面 ===

void UIManager::showTaskMenu() {
    clearScreen();
    printHeader("📋 任务管理");
    
    // 显示任务统计摘要
    cout << COLOR_BRIGHT_CYAN << "  ╭───────────────────────────────────────────────────╮\n";
    cout << "  │  " << COLOR_BRIGHT_WHITE << "📊 任务概览" << setw(40) << right << "" << COLOR_BRIGHT_CYAN << "│\n";
    cout << "  │  " << COLOR_BRIGHT_GREEN << "✓ 今日完成: " << COLOR_BRIGHT_WHITE << "0" 
         << COLOR_RESET << " │ " << COLOR_BRIGHT_YELLOW << "⏳ 进行中: " << COLOR_BRIGHT_WHITE << "0" 
         << COLOR_RESET << " │ " << COLOR_BRIGHT_RED << "⚠ 过期: " << COLOR_BRIGHT_WHITE << "0" 
         << setw(5) << right << "" << COLOR_BRIGHT_CYAN << "│\n";
    cout << "  ╰───────────────────────────────────────────────────╯" << COLOR_RESET << "\n";
    
    vector<string> options = {
        "✨ 创建新任务 - 添加新的待办事项",
        "📜 查看所有任务 - 浏览任务列表",
        "✏️  更新任务 - 修改任务信息",
        "🗑️  删除任务 - 移除不需要的任务",
        "✅ 完成任务 - 标记任务为已完成"
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
    
    cout << "\n";
    cout << COLOR_BRIGHT_CYAN << "  ╭───────────────────────────────────────────────────╮\n";
    cout << "  │  " << COLOR_BRIGHT_YELLOW << "🔧 功能提示" << setw(39) << right << "" << COLOR_BRIGHT_CYAN << "│\n";
    cout << "  ╰───────────────────────────────────────────────────╯" << COLOR_RESET << "\n";
    
    displayInfo(MSG_MODULE_IN_DEVELOPMENT);
    displayWarning("需要等待成员C完成TaskManager模块");
    
    pause();
}

void UIManager::listTasks() {
    clearScreen();
    printHeader("📋 任务列表");
    
    displayInfo(MSG_MODULE_IN_DEVELOPMENT);
    displayWarning("需要等待成员C完成TaskManager模块");
    
    pause();
}

void UIManager::updateTask() {
    clearScreen();
    printHeader("✏️  更新任务");
    
    displayInfo(MSG_MODULE_IN_DEVELOPMENT);
    
    pause();
}

void UIManager::deleteTask() {
    clearScreen();
    printHeader("🗑️  删除任务");
    
    displayInfo(MSG_MODULE_IN_DEVELOPMENT);
    
    pause();
}

void UIManager::completeTask() {
    clearScreen();
    printHeader("✅ 完成任务");
    
    displayInfo(MSG_MODULE_IN_DEVELOPMENT);
    displayInfo("💡 提示: 完成任务后会自动获得经验值奖励哦！");
    
    pause();
}

// === 项目管理界面 ===

void UIManager::showProjectMenu() {
    clearScreen();
    printHeader("📁 项目管理");
    
    // 项目统计摘要
    int projectCount = projectManager->getProjectCount();
    int activeCount = projectManager->getActiveProjectCount();
    
    cout << COLOR_BRIGHT_CYAN << "  ╭───────────────────────────────────────────────────╮\n";
    cout << "  │  " << COLOR_BRIGHT_WHITE << "📊 项目概览" << setw(40) << right << "" << COLOR_BRIGHT_CYAN << "│\n";
    cout << "  │  " << COLOR_BRIGHT_BLUE << "📁 总项目: " << COLOR_BRIGHT_WHITE << projectCount 
         << COLOR_RESET << " │ " << COLOR_BRIGHT_GREEN << "🔥 活跃: " << COLOR_BRIGHT_WHITE << activeCount 
         << COLOR_RESET << " │ " << COLOR_BRIGHT_YELLOW << "📦 归档: " << COLOR_BRIGHT_WHITE << (projectCount - activeCount)
         << setw(5) << right << "" << COLOR_BRIGHT_CYAN << "│\n";
    cout << "  ╰───────────────────────────────────────────────────╯" << COLOR_RESET << "\n";
    
    vector<string> options = {
        "✨ 创建新项目 - 开始一个新项目",
        "📜 查看所有项目 - 浏览项目列表",
        "🔍 查看项目详情 - 深入了解项目",
        "✏️  更新项目 - 修改项目信息",
        "🗑️  删除项目 - 移除不需要的项目"
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
    
    cout << "\n";
    cout << COLOR_BRIGHT_CYAN << "  ╭───────────────────────────────────────────────────╮\n";
    cout << "  │  " << COLOR_BRIGHT_WHITE << "请填写项目信息" << setw(35) << right << "" << COLOR_BRIGHT_CYAN << "│\n";
    cout << "  ╰───────────────────────────────────────────────────╯" << COLOR_RESET << "\n\n";
    
    string name = getInput("项目名称: ");
    if (name.empty()) {
        displayError("项目名称不能为空！");
        pause();
        return;
    }
    
    string desc = getInput("项目描述: ");
    string color = getInput("颜色标签 (如 #4CAF50，直接回车使用默认): ");
    if (color.empty()) color = DEFAULT_PROJECT_COLOR;
    
    Project project(name, desc, color);
    int id = projectManager->createProject(project);
    
    cout << "\n";
    displaySuccess("🎉 项目创建成功！ID: " + to_string(id));
    
    pause();
}

void UIManager::listProjects() {
    clearScreen();
    printHeader("📁 项目列表");
    
    vector<Project*> projects = projectManager->getAllProjects();
    
    if (projects.empty()) {
        cout << "\n";
        cout << COLOR_BRIGHT_CYAN << "  ╭───────────────────────────────────────────────────╮\n";
        cout << "  │  " << COLOR_BRIGHT_YELLOW << "📭 暂无项目，快去创建一个吧！" << setw(20) << right << "" << COLOR_BRIGHT_CYAN << "│\n";
        cout << "  ╰───────────────────────────────────────────────────╯" << COLOR_RESET << "\n";
    } else {
        cout << "\n";
        for (Project* p : projects) {
            double progress = p->getProgress();
            
            cout << COLOR_BRIGHT_CYAN << "  ╭───────────────────────────────────────────────────╮\n";
            cout << "  │  " << COLOR_BRIGHT_BLUE << "📁 ID: " << p->getId() 
                 << COLOR_BRIGHT_WHITE << BOLD << " " << p->getName() << COLOR_RESET;
            // 使用固定填充以避免Unicode宽度问题
            cout << COLOR_BRIGHT_CYAN << "\n";
            
            // 进度条
            cout << "  │  " << COLOR_BRIGHT_WHITE << "进度: ";
            int barWidth = 25;
            int filled = static_cast<int>(progress * barWidth);
            cout << COLOR_BRIGHT_GREEN;
            for (int i = 0; i < filled; i++) cout << "█";
            cout << COLOR_WHITE;
            for (int i = filled; i < barWidth; i++) cout << "░";
            cout << " " << fixed << setprecision(0) << (progress * 100) << "%" << COLOR_BRIGHT_CYAN << "\n";
            
            // 描述 - 限制显示长度避免截断Unicode
            string desc = p->getDescription();
            if (desc.length() > 35) {
                desc = desc.substr(0, 35) + "...";
            }
            cout << "  │  " << COLOR_WHITE << "📝 " << desc << COLOR_BRIGHT_CYAN << "\n";
            cout << "  ╰───────────────────────────────────────────────────╯" << COLOR_RESET << "\n";
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
        cout << COLOR_BRIGHT_CYAN << "  ╔═══════════════════════════════════════════════════════╗\n";
        cout << "  ║  " << COLOR_BRIGHT_WHITE << BOLD << "📁 " << setw(50) << left << p->getName() << COLOR_RESET << COLOR_BRIGHT_CYAN << "║\n";
        cout << "  ╠═══════════════════════════════════════════════════════╣\n";
        cout << "  ║  " << COLOR_WHITE << "📝 描述: " << setw(44) << left << p->getDescription() << COLOR_BRIGHT_CYAN << "║\n";
        cout << "  ║  " << COLOR_WHITE << "🎨 颜色: " << setw(44) << left << p->getColorLabel() << COLOR_BRIGHT_CYAN << "║\n";
        
        // 进度条
        double progress = p->getProgress();
        cout << "  ║  " << COLOR_WHITE << "📈 进度: ";
        int barWidth = 20;
        int filled = static_cast<int>(progress * barWidth);
        cout << COLOR_BRIGHT_GREEN;
        for (int i = 0; i < filled; i++) cout << "█";
        cout << COLOR_WHITE;
        for (int i = filled; i < barWidth; i++) cout << "░";
        cout << " " << fixed << setprecision(1) << (progress * 100) << "%";
        cout << setw(12) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
        
        cout << "  ║  " << COLOR_WHITE << "📋 任务: " << p->getCompletedTasks() << "/" << p->getTotalTasks() << " 已完成";
        cout << setw(30) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
        cout << "  ║  " << COLOR_WHITE << "🎯 目标: " << setw(44) << left << p->getTargetDate() << COLOR_BRIGHT_CYAN << "║\n";
        cout << "  ║  " << COLOR_WHITE << "📅 创建: " << setw(44) << left << p->getCreatedDate() << COLOR_BRIGHT_CYAN << "║\n";
        cout << "  ║  " << COLOR_WHITE << "💫 状态: " << setw(44) << left << (p->isArchived() ? "📦 已归档" : "🔥 活跃") << COLOR_BRIGHT_CYAN << "║\n";
        cout << "  ╚═══════════════════════════════════════════════════════╝" << COLOR_RESET << "\n";
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
            displaySuccess("🗑️  项目删除成功！");
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
    
    // 快速统计摘要
    int todayTasks = statsAnalyzer->getTasksCompletedToday();
    int weekTasks = statsAnalyzer->getTasksCompletedThisWeek();
    int streak = statsAnalyzer->getCurrentStreak();
    
    cout << COLOR_BRIGHT_CYAN << "  ╭───────────────────────────────────────────────────╮\n";
    cout << "  │  " << COLOR_BRIGHT_WHITE << "📈 快速统计" << setw(40) << right << "" << COLOR_BRIGHT_CYAN << "│\n";
    cout << "  │  " << COLOR_BRIGHT_GREEN << "📅 今日: " << COLOR_BRIGHT_WHITE << todayTasks 
         << COLOR_RESET << " │ " << COLOR_BRIGHT_BLUE << "📊 本周: " << COLOR_BRIGHT_WHITE << weekTasks 
         << COLOR_RESET << " │ " << COLOR_BRIGHT_YELLOW << "🔥 连续: " << COLOR_BRIGHT_WHITE << streak << "天"
         << setw(3) << right << "" << COLOR_BRIGHT_CYAN << "│\n";
    cout << "  ╰───────────────────────────────────────────────────╯" << COLOR_RESET << "\n";
    
    vector<string> options = {
        "📈 统计数据总览 - 查看综合数据",
        "📅 每日报告 - 今日表现",
        "📊 每周报告 - 本周总结",
        "📆 每月报告 - 月度分析",
        "🔥 任务完成热力图 - 可视化您的努力"
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
    
    // 游戏化状态
    int level = xpSystem->getCurrentLevel();
    int totalXP = xpSystem->getTotalXP();
    int achievements = statsAnalyzer->getAchievementsUnlocked();
    int challenges = statsAnalyzer->getChallengesCompleted();
    
    cout << COLOR_BRIGHT_CYAN << "  ╭───────────────────────────────────────────────────╮\n";
    cout << "  │  " << COLOR_BRIGHT_WHITE << "🎮 游戏化状态" << setw(38) << right << "" << COLOR_BRIGHT_CYAN << "│\n";
    cout << "  │  " << COLOR_BRIGHT_MAGENTA << "⭐ Lv." << level << COLOR_RESET 
         << " │ " << COLOR_BRIGHT_YELLOW << "XP: " << totalXP 
         << " │ " << COLOR_BRIGHT_GREEN << "🏆 " << achievements << "成就"
         << " │ " << COLOR_BRIGHT_BLUE << "🎯 " << challenges << "挑战"
         << setw(2) << right << "" << COLOR_BRIGHT_CYAN << "│\n";
    cout << "  ╰───────────────────────────────────────────────────╯" << COLOR_RESET << "\n";
    
    vector<string> options = {
        "⭐ 经验值和等级 - 查看您的成长之路",
        "🏆 成就系统 - 解锁荣誉徽章",
        "🎯 挑战系统 - 接受挑战获取奖励"
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
    
    int level = xpSystem->getCurrentLevel();
    int totalXP = xpSystem->getTotalXP();
    int nextLevelXP = xpSystem->getXPForNextLevel();
    string title = xpSystem->getCurrentLevelTitle();
    double progress = (nextLevelXP > 0) ? static_cast<double>(totalXP) / nextLevelXP : 0.0;
    
    cout << "\n";
    cout << COLOR_BRIGHT_CYAN << "  ╔═══════════════════════════════════════════════════════╗\n";
    cout << "  ║                                                       ║\n";
    cout << "  ║     " << COLOR_BRIGHT_YELLOW << "🌟" << COLOR_BRIGHT_WHITE << BOLD << "  等级 " << level << COLOR_RESET << COLOR_BRIGHT_CYAN << "  【" << COLOR_BRIGHT_MAGENTA << title << COLOR_BRIGHT_CYAN << "】" << setw(22) << right << "" << "║\n";
    cout << "  ║                                                       ║\n";
    
    // 大进度条
    cout << "  ║     " << COLOR_BRIGHT_YELLOW << "⚡ 经验值进度" << COLOR_RESET << setw(35) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ║     ";
    int barWidth = 40;
    int filled = static_cast<int>(progress * barWidth);
    cout << COLOR_BRIGHT_GREEN;
    for (int i = 0; i < filled; i++) cout << "█";
    cout << COLOR_WHITE;
    for (int i = filled; i < barWidth; i++) cout << "░";
    cout << "     " << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ║     " << COLOR_BRIGHT_WHITE << "XP: " << totalXP << " / " << nextLevelXP;
    cout << "  (" << fixed << setprecision(1) << (progress * 100) << "%)";
    cout << setw(23) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ║                                                       ║\n";
    
    // XP获取方式
    cout << "  ╠═══════════════════════════════════════════════════════╣\n";
    cout << "  ║  " << COLOR_BRIGHT_YELLOW << "💡 如何获取经验值?" << COLOR_RESET << setw(34) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ║  " << COLOR_WHITE << "  • 完成任务: +10~30 XP (根据优先级)" << setw(15) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ║  " << COLOR_WHITE << "  • 完成番茄钟: +5 XP" << setw(29) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ║  " << COLOR_WHITE << "  • 连续打卡: +2~20 XP (连续天数)" << setw(17) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ║  " << COLOR_WHITE << "  • 完成挑战: 奖励XP" << setw(30) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ║  " << COLOR_WHITE << "  • 解锁成就: 奖励XP" << setw(30) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ║                                                       ║\n";
    cout << "  ╚═══════════════════════════════════════════════════════╝" << COLOR_RESET << "\n";
    
    pause();
}

void UIManager::showAchievements() {
    clearScreen();
    printHeader("🏆 成就系统");
    
    int unlocked = statsAnalyzer->getAchievementsUnlocked();
    
    cout << "\n";
    cout << COLOR_BRIGHT_CYAN << "  ╭───────────────────────────────────────────────────╮\n";
    cout << "  │  " << COLOR_BRIGHT_YELLOW << "🏆 已解锁成就: " << COLOR_BRIGHT_WHITE << BOLD << unlocked << COLOR_RESET 
         << " 个" << setw(30) << right << "" << COLOR_BRIGHT_CYAN << "│\n";
    cout << "  ╰───────────────────────────────────────────────────╯" << COLOR_RESET << "\n\n";
    
    // 成就预览
    cout << COLOR_BRIGHT_WHITE << "  📋 成就列表预览:\n" << COLOR_RESET;
    cout << COLOR_WHITE << "  ┌─────────────────────────────────────────────────────┐\n";
    cout << "  │  🔒 初次打卡 - 完成第一个任务                       │\n";
    cout << "  │  🔒 连续7天 - 连续打卡7天                           │\n";
    cout << "  │  🔒 番茄大师 - 完成100个番茄钟                      │\n";
    cout << "  │  🔒 项目达人 - 完成10个项目                         │\n";
    cout << "  │  🔒 效率之星 - 单日完成20个任务                     │\n";
    cout << "  └─────────────────────────────────────────────────────┘" << COLOR_RESET << "\n\n";
    
    displayInfo("成就系统详细功能开发中...");
    
    pause();
}

void UIManager::showChallenges() {
    clearScreen();
    printHeader("🎯 挑战系统");
    
    int completed = statsAnalyzer->getChallengesCompleted();
    
    cout << "\n";
    cout << COLOR_BRIGHT_CYAN << "  ╭───────────────────────────────────────────────────╮\n";
    cout << "  │  " << COLOR_BRIGHT_YELLOW << "🎯 已完成挑战: " << COLOR_BRIGHT_WHITE << BOLD << completed << COLOR_RESET 
         << " 个" << setw(30) << right << "" << COLOR_BRIGHT_CYAN << "│\n";
    cout << "  ╰───────────────────────────────────────────────────╯" << COLOR_RESET << "\n\n";
    
    // 挑战预览
    cout << COLOR_BRIGHT_WHITE << "  📋 挑战列表预览:\n" << COLOR_RESET;
    cout << COLOR_WHITE << "  ┌─────────────────────────────────────────────────────┐\n";
    cout << "  │  " << COLOR_BRIGHT_GREEN << "🌅" << COLOR_WHITE << " 日常挑战: 今日完成5个任务 (+50 XP)              │\n";
    cout << "  │  " << COLOR_BRIGHT_BLUE << "📊" << COLOR_WHITE << " 周挑战: 本周完成20个任务 (+200 XP)              │\n";
    cout << "  │  " << COLOR_BRIGHT_MAGENTA << "🌟" << COLOR_WHITE << " 月挑战: 本月完成100个任务 (+500 XP)            │\n";
    cout << "  │  " << COLOR_BRIGHT_YELLOW << "🍅" << COLOR_WHITE << " 番茄挑战: 今日完成4个番茄钟 (+30 XP)           │\n";
    cout << "  └─────────────────────────────────────────────────────┘" << COLOR_RESET << "\n\n";
    
    displayInfo("挑战系统详细功能开发中...");
    
    pause();
}

// === 设置界面 ===

void UIManager::showSettingsMenu() {
    clearScreen();
    printHeader("⚙️  系统设置");
    
    vector<string> options = {
        "📋 查看设置 - 查看当前配置",
        "✏️  修改设置 - 个性化您的体验"
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
    
    cout << "\n";
    cout << COLOR_BRIGHT_CYAN << "  ╔═══════════════════════════════════════════════════════╗\n";
    cout << "  ║  " << COLOR_BRIGHT_WHITE << "🍅 番茄钟设置" << setw(40) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ╠═══════════════════════════════════════════════════════╣\n";
    cout << "  ║  " << COLOR_WHITE << "• 工作时长: 25 分钟" << setw(33) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ║  " << COLOR_WHITE << "• 短休息: 5 分钟" << setw(36) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ║  " << COLOR_WHITE << "• 长休息: 15 分钟" << setw(35) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ║  " << COLOR_WHITE << "• 长休息间隔: 4 个番茄钟" << setw(27) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ╠═══════════════════════════════════════════════════════╣\n";
    cout << "  ║  " << COLOR_BRIGHT_WHITE << "🔔 通知设置" << setw(42) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ╠═══════════════════════════════════════════════════════╣\n";
    cout << "  ║  " << COLOR_WHITE << "• 声音提醒: " << COLOR_BRIGHT_GREEN << "开启" << setw(34) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ║  " << COLOR_WHITE << "• 通知推送: " << COLOR_BRIGHT_GREEN << "开启" << setw(34) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ╠═══════════════════════════════════════════════════════╣\n";
    cout << "  ║  " << COLOR_BRIGHT_WHITE << "🎨 界面设置" << setw(42) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ╠═══════════════════════════════════════════════════════╣\n";
    cout << "  ║  " << COLOR_WHITE << "• 主题: " << COLOR_BRIGHT_CYAN << "默认" << setw(38) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ║  " << COLOR_WHITE << "• 语言: " << COLOR_BRIGHT_WHITE << "中文" << setw(38) << right << "" << COLOR_BRIGHT_CYAN << "║\n";
    cout << "  ╚═══════════════════════════════════════════════════════╝" << COLOR_RESET << "\n";
    
    pause();
}

void UIManager::updateSettings() {
    clearScreen();
    printHeader("✏️  修改设置");
    
    displayInfo(MSG_SETTINGS_IN_DEVELOPMENT);
    
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
    cout << "\n";
    cout << COLOR_BRIGHT_RED << "  ┌─ ❌ 错误 ─────────────────────────────────────────┐\n";
    cout << "  │  " << COLOR_WHITE << setw(49) << left << error << COLOR_BRIGHT_RED << "│\n";
    cout << "  └─────────────────────────────────────────────────────┘" << COLOR_RESET << "\n";
}

void UIManager::displaySuccess(const string& msg) {
    cout << "\n";
    cout << COLOR_BRIGHT_GREEN << "  ┌─ ✅ 成功 ─────────────────────────────────────────┐\n";
    cout << "  │  " << COLOR_WHITE << setw(49) << left << msg << COLOR_BRIGHT_GREEN << "│\n";
    cout << "  └─────────────────────────────────────────────────────┘" << COLOR_RESET << "\n";
}

void UIManager::displayWarning(const string& warning) {
    cout << "\n";
    cout << COLOR_BRIGHT_YELLOW << "  ┌─ ⚠️  警告 ─────────────────────────────────────────┐\n";
    cout << "  │  " << COLOR_WHITE << setw(49) << left << warning << COLOR_BRIGHT_YELLOW << "│\n";
    cout << "  └─────────────────────────────────────────────────────┘" << COLOR_RESET << "\n";
}

void UIManager::displayInfo(const string& info) {
    cout << "\n";
    cout << COLOR_BRIGHT_CYAN << "  ┌─ ℹ️  提示 ─────────────────────────────────────────┐\n";
    cout << "  │  " << COLOR_WHITE << setw(49) << left << info << COLOR_BRIGHT_CYAN << "│\n";
    cout << "  └─────────────────────────────────────────────────────┘" << COLOR_RESET << "\n";
}
