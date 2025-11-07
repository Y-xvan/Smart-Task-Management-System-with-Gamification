#pragma once
#include <vector>
#include <string>
#include <chrono>
#include "include/database/DatabaseManager.h" // 包含队友的头文件

// 提醒数据结构 - 与数据库表结构对应
struct Reminder {
    int id;
    std::string title;
    std::string message;
    std::string trigger_time;
    std::string recurrence; // "once", "daily", "weekly", "monthly"
    bool triggered;
    int task_id;
    bool enabled;
    std::string last_triggered;
    
    Reminder(int id, const std::string& title, const std::string& msg, 
             const std::string& time, const std::string& rule, int task_id = 0)
        : id(id), title(title), message(msg), trigger_time(time), 
          recurrence(rule), triggered(false), task_id(task_id), enabled(true) {}
};

class ReminderSystem {
private:
    std::vector<Reminder> reminders;
    DatabaseManager* dbManager; // 使用队友的DatabaseManager
    
public:
    ReminderSystem(DatabaseManager* dm);
    
    // 核心方法
    void initialize();
    void checkDueReminders();
    void addReminder(const std::string& title, const std::string& message,
                    const std::string& time, const std::string& rule = "once",
                    int task_id = 0);
    void displayAllReminders();
    void displayPendingReminders();
    
    // 工具方法
    bool isReminderDue(const Reminder& reminder) const;
    void processRecurringReminder(const Reminder& reminder);
    std::string calculateNextTriggerTime(const Reminder& reminder) const;
    bool loadRemindersFromDB();
    bool saveReminderToDB(const Reminder& reminder);
    bool updateReminderInDB(const Reminder& reminder);
    
    // 时间工具方法
    std::string getCurrentTime() const;
    std::time_t parseTimeString(const std::string& timeStr) const;
    std::string formatTime(std::time_t time) const;
};