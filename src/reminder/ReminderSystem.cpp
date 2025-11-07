#include "ReminderSystem.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

ReminderSystem::ReminderSystem(DatabaseManager* dm) : dbManager(dm) {
    initialize();
}

void ReminderSystem::initialize() {
    if (loadRemindersFromDB()) {
        std::cout << "提醒系统初始化完成，共加载 " << reminders.size() << " 个提醒\n";
    } else {
        std::cout << "提醒系统初始化失败\n";
    }
}

bool ReminderSystem::loadRemindersFromDB() {
    // 这里需要队友在DatabaseManager中实现getAllReminders方法
    // 暂时用模拟数据
    std::cout << "从数据库加载提醒数据...\n";
    
    // 添加一些示例提醒用于测试
    reminders.emplace_back(1, "晨会提醒", "记得参加每日晨会", 
                          "2024-01-15 09:00:00", "daily");
    reminders.emplace_back(2, "任务截止", "项目报告截止日期", 
                          "2024-01-20 18:00:00", "once", 101);
    
    // TODO: 当队友实现getAllReminders后，替换为：
    // reminders = dbManager->getAllReminders();
    
    return true;
}

bool ReminderSystem::saveReminderToDB(const Reminder& reminder) {
    // 这里需要队友在DatabaseManager中实现addReminder方法
    std::cout << "保存提醒到数据库: " << reminder.title << "\n";
    // TODO: 调用队友的数据库方法
    // return dbManager->addReminder(reminder);
    return true;
}

bool ReminderSystem::updateReminderInDB(const Reminder& reminder) {
    // 这里需要队友在DatabaseManager中实现updateReminder方法
    std::cout << "更新提醒到数据库: " << reminder.title << "\n";
    // TODO: 调用队友的数据库方法
    // return dbManager->updateReminder(reminder);
    return true;
}

void ReminderSystem::checkDueReminders() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    
    std::cout << "=== 检查到期提醒 (" << getCurrentTime() << ") ===\n";
    int triggeredCount = 0;
    
    for (auto& reminder : reminders) {
        if (reminder.enabled && !reminder.triggered && isReminderDue(reminder)) {
            // 触发提醒
            std::cout << "🔔 提醒: " << reminder.title << "\n";
            std::cout << "   " << reminder.message << "\n";
            if (reminder.task_id > 0) {
                std::cout << "   关联任务ID: " << reminder.task_id << "\n";
            }
            std::cout << "   触发时间: " << reminder.trigger_time << "\n\n";
            
            reminder.triggered = true;
            reminder.last_triggered = getCurrentTime();
            
            // 更新数据库
            updateReminderInDB(reminder);
            triggeredCount++;
            
            // 处理重复提醒
            if (reminder.recurrence != "once") {
                processRecurringReminder(reminder);
            }
        }
    }
    
    if (triggeredCount == 0) {
        std::cout << "暂无到期提醒\n";
    } else {
        std::cout << "共触发 " << triggeredCount << " 个提醒\n";
    }
    std::cout << "===================\n\n";
}

bool ReminderSystem::isReminderDue(const Reminder& reminder) const {
    // 解析提醒时间
    std::time_t reminderTime = parseTimeString(reminder.trigger_time);
    if (reminderTime == -1) return false;
    
    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    
    // 简单判断：如果提醒时间小于等于当前时间，则认为到期
    // 实际项目中可能需要更复杂的时间比较逻辑
    return reminderTime <= now_time;
}

void ReminderSystem::processRecurringReminder(const Reminder& reminder) {
    std::string nextTime = calculateNextTriggerTime(reminder);
    
    // 创建新的提醒记录
    int newId = reminders.empty() ? 1 : reminders.back().id + 1;
    Reminder newReminder(newId, reminder.title, reminder.message, 
                        nextTime, reminder.recurrence, reminder.task_id);
    
    reminders.push_back(newReminder);
    saveReminderToDB(newReminder);
    
    std::cout << "已创建下一次提醒，时间: " << nextTime << "\n";
}

std::string ReminderSystem::calculateNextTriggerTime(const Reminder& reminder) const {
    std::time_t currentTime = parseTimeString(reminder.trigger_time);
    if (currentTime == -1) return "";
    
    std::time_t nextTime = currentTime;
    
    if (reminder.recurrence == "daily") {
        nextTime += 24 * 60 * 60; // 增加1天
    } else if (reminder.recurrence == "weekly") {
        nextTime += 7 * 24 * 60 * 60; // 增加1周
    } else if (reminder.recurrence == "monthly") {
        // 简单实现：增加30天
        nextTime += 30 * 24 * 60 * 60;
    }
    
    return formatTime(nextTime);
}

void ReminderSystem::addReminder(const std::string& title, const std::string& message,
                                const std::string& time, const std::string& rule,
                                int task_id) {
    int newId = reminders.empty() ? 1 : reminders.back().id + 1;
    Reminder newReminder(newId, title, message, time, rule, task_id);
    
    reminders.push_back(newReminder);
    saveReminderToDB(newReminder);
    
    std::cout << "✅ 已添加提醒: " << title << " (时间: " << time << ", 重复: " << rule << ")\n";
}

void ReminderSystem::displayAllReminders() {
    std::cout << "=== 所有提醒 (" << reminders.size() << "个) ===\n";
    for (const auto& reminder : reminders) {
        std::cout << (reminder.triggered ? "✅ " : "⏰ ");
        std::cout << "ID: " << reminder.id;
        std::cout << " | 时间: " << reminder.trigger_time;
        std::cout << " | 重复: " << reminder.recurrence;
        std::cout << " | 状态: " << (reminder.enabled ? "启用" : "禁用") << "\n";
        std::cout << "   标题: " << reminder.title << "\n";
        std::cout << "   内容: " << reminder.message;
        if (reminder.task_id > 0) {
            std::cout << " (任务ID: " << reminder.task_id << ")";
        }
        std::cout << "\n";
        if (reminder.triggered && !reminder.last_triggered.empty()) {
            std::cout << "   上次触发: " << reminder.last_triggered << "\n";
        }
        std::cout << "\n";
    }
    std::cout << "================\n\n";
}

void ReminderSystem::displayPendingReminders() {
    std::cout << "=== 待处理提醒 ===\n";
    int count = 0;
    for (const auto& reminder : reminders) {
        if (reminder.enabled && !reminder.triggered) {
            std::cout << "⏰ ID: " << reminder.id;
            std::cout << " | 时间: " << reminder.trigger_time;
            std::cout << " | 重复: " << reminder.recurrence << "\n";
            std::cout << "   标题: " << reminder.title << "\n";
            count++;
        }
    }
    if (count == 0) {
        std::cout << "暂无待处理提醒\n";
    } else {
        std::cout << "共 " << count << " 个待处理提醒\n";
    }
    std::cout << "==================\n\n";
}

// 时间工具方法
std::string ReminderSystem::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    return formatTime(now_time);
}

std::time_t ReminderSystem::parseTimeString(const std::string& timeStr) const {
    std::tm tm = {};
    std::istringstream ss(timeStr);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        return -1;
    }
    return std::mktime(&tm);
}

std::string ReminderSystem::formatTime(std::time_t time) const {
    std::tm* timeinfo = std::localtime(&time);
    std::stringstream ss;
    ss << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}