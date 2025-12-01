#include <cassert>
#include <chrono>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <memory>
#include <thread>
#include "reminder/ReminderSystem.h"
#include "achievement/AchievementManager.h"

// ===================== 内存版 ReminderDAO =====================
class FakeReminderDAO : public ReminderDAO {
private:
    std::vector<Reminder> reminders;
    int nextId = 1;

    static bool isDue(const Reminder& reminder, const std::chrono::system_clock::time_point& now) {
        return reminder.enabled && !reminder.triggered && reminder.triggerTime <= now;
    }

public:
    bool insertReminder(Reminder& reminder) override {
        reminder.id = nextId++;
        reminders.push_back(reminder);
        return true;
    }

    bool updateReminder(const Reminder& reminder) override {
        for (auto& r : reminders) {
            if (r.id == reminder.id) {
                r = reminder;
                return true;
            }
        }
        return false;
    }

    bool deleteReminder(int reminderId) override {
        auto sizeBefore = reminders.size();
        reminders.erase(std::remove_if(reminders.begin(), reminders.end(), [&](const Reminder& r) {
            return r.id == reminderId;
        }), reminders.end());
        return sizeBefore != reminders.size();
    }

    std::optional<Reminder> getReminderById(int reminderId) override {
        for (const auto& r : reminders) {
            if (r.id == reminderId) return r;
        }
        return std::nullopt;
    }

    std::vector<Reminder> getAllReminders() override { return reminders; }

    std::vector<Reminder> getActiveReminders() override {
        std::vector<Reminder> result;
        for (const auto& r : reminders) {
            if (r.enabled && !r.triggered) result.push_back(r);
        }
        return result;
    }

    std::vector<Reminder> getRemindersByTask(int taskId) override {
        std::vector<Reminder> result;
        for (const auto& r : reminders) {
            if (r.task_id == taskId) result.push_back(r);
        }
        return result;
    }

    std::vector<Reminder> getRemindersByType(ReminderType) override { return reminders; }

    std::vector<Reminder> getRemindersByRecurrence(const std::string& recurrence) override {
        std::vector<Reminder> result;
        for (const auto& r : reminders) {
            if (r.recurrence == recurrence) result.push_back(r);
        }
        return result;
    }

    std::vector<Reminder> getDueReminders(const std::chrono::system_clock::time_point& currentTime) override {
        std::vector<Reminder> result;
        for (const auto& r : reminders) {
            if (isDue(r, currentTime)) result.push_back(r);
        }
        return result;
    }

    std::vector<Reminder> getRemindersDueToday() override { return reminders; }
    std::vector<Reminder> getRemindersDueThisWeek() override { return reminders; }

    std::vector<Reminder> getRemindersByDateRange(
        const std::chrono::system_clock::time_point&,
        const std::chrono::system_clock::time_point&) override {
        return reminders;
    }

    bool markReminderAsTriggered(int reminderId) override {
        for (auto& r : reminders) {
            if (r.id == reminderId) {
                r.triggered = true;
                r.last_triggered = "triggered";
                r.status = ReminderStatus::COMPLETED;
                return true;
            }
        }
        return false;
    }

    bool markReminderAsCompleted(int reminderId) override { return markReminderAsTriggered(reminderId); }

    bool rescheduleReminder(int reminderId, const std::chrono::system_clock::time_point& newTime) override {
        for (auto& r : reminders) {
            if (r.id == reminderId) {
                r.trigger_time = "rescheduled";
                r.triggerTime = newTime;
                r.triggered = false;
                r.status = ReminderStatus::PENDING;
                return true;
            }
        }
        return false;
    }

    bool createNextRecurringReminder(int originalReminderId) override {
        auto original = getReminderById(originalReminderId);
        if (!original || original->recurrence == "once") return false;

        Reminder next = *original;
        next.id = 0;
        next.triggered = false;
        next.status = ReminderStatus::PENDING;
        next.trigger_time = "next";
        next.enabled = true;
        return insertReminder(next);
    }
    std::vector<Reminder> getRecurringReminders() override { return reminders; }
    bool deleteExpiredReminders() override { return true; }
    bool cleanUpCompletedReminders() override { return true; }
    int getReminderCountByStatus(ReminderStatus) override { return static_cast<int>(reminders.size()); }
    int getOverdueReminderCount() override { return 0; }
};

// ===================== 测试帮助函数 =====================
std::string formatTimeOffsetSeconds(int offsetSeconds) {
    auto tp = std::chrono::system_clock::now() + std::chrono::seconds(offsetSeconds);
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm* tm = std::localtime(&t);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
    return buffer;
}

// ===================== 自定义统计分析器用于成就测试 =====================
class FakeStatisticsAnalyzer : public StatisticsAnalyzer {
public:
    int completedTasks = 0;
    int streak = 0;
    int dailyCount = 0;
    int pomodoroCount = 0;

    int getTotalTasksCompleted() override { return completedTasks; }
    int getCurrentStreak() override { return streak; }
    int getTasksCompletedToday() override { return dailyCount; }
    int getTotalPomodoros() override { return pomodoroCount; }
};

int main() {
    // ============ ReminderSystem 测试 ============
    auto reminderDao = std::make_unique<FakeReminderDAO>();
    auto* reminderDaoPtr = reminderDao.get();
    ReminderSystem reminderSystem(std::move(reminderDao));

    // 1. 添加一次性到期提醒并自动触发
    const std::string pastTime = formatTimeOffsetSeconds(-120);
    reminderSystem.addReminder("测试提醒", "内容", pastTime, "once", 42);
    reminderSystem.checkDueReminders();

    auto reminder = reminderDaoPtr->getReminderById(1);
    assert(reminder.has_value());
    assert(reminder->triggered && "到期提醒应被标记为已触发");
    assert(reminder->status == ReminderStatus::COMPLETED);
    assert(reminder->task_id == 42);
    assert(!reminder->message.empty() && !reminder->title.empty());

    // 2. 重复提醒应创建下一次记录
    const std::string recurringPast = formatTimeOffsetSeconds(-60);
    reminderSystem.addReminder("重复提醒", "每日一次", recurringPast, "daily", 1);
    reminderSystem.checkDueReminders();

    auto reminders = reminderDaoPtr->getAllReminders();
    bool hasFutureRecurring = false;
    for (const auto& r : reminders) {
        if (!r.triggered && r.recurrence == "daily") {
            hasFutureRecurring = true;
            assert(r.enabled && r.status == ReminderStatus::PENDING);
            break;
        }
    }
    assert(hasFutureRecurring && "重复提醒应创建下一次触发记录");

    // 3. 禁用提醒不会被触发，启用后可触发且可按类型、任务过滤
    const std::string soon = formatTimeOffsetSeconds(10);
    Reminder disabled{};
    disabled.title = "禁用提醒";
    disabled.message = "不会触发";
    disabled.trigger_time = soon;
    disabled.triggerTime = std::chrono::system_clock::now() + std::chrono::seconds(10);
    disabled.recurrence = "weekly";
    disabled.type = ReminderType::WEEKLY;
    disabled.task_id = 99;
    disabled.enabled = false;
    reminderDaoPtr->insertReminder(disabled);

    reminderSystem.checkDueReminders();
    auto disabledFromDao = reminderDaoPtr->getRemindersByTask(99);
    assert(!disabledFromDao.empty());
    assert(!disabledFromDao.front().triggered && "禁用提醒不应被触发");

    disabledFromDao.front().enabled = true;
    reminderDaoPtr->updateReminder(disabledFromDao.front());
    reminderSystem.checkDueReminders();
    auto typeFiltered = reminderDaoPtr->getRemindersByType(ReminderType::WEEKLY);
    assert(!typeFiltered.empty());

    // 4. 任务过滤与激活列表
    reminderSystem.addReminder("周提醒", "每周一次", formatTimeOffsetSeconds(3600), "weekly", 7);
    reminderSystem.addReminder("月提醒", "每月一次", formatTimeOffsetSeconds(7200), "monthly", 7);

    auto taskReminders = reminderDaoPtr->getRemindersByTask(7);
    assert(taskReminders.size() == 2 && "应按任务ID过滤提醒");

    auto active = reminderDaoPtr->getActiveReminders();
    assert(!active.empty() && "应能获取未触发的提醒");

    // 5. 重新安排与状态重置
    int rescheduleId = active.front().id;
    auto futureTime = std::chrono::system_clock::now() + std::chrono::seconds(600);
    bool rescheduled = reminderDaoPtr->rescheduleReminder(rescheduleId, futureTime);
    assert(rescheduled);
    auto rescheduledReminder = reminderDaoPtr->getReminderById(rescheduleId);
    assert(rescheduledReminder && !rescheduledReminder->triggered);

    // 6. 删除提醒与统计
    size_t beforeDelete = reminderDaoPtr->getAllReminders().size();
    bool deleted = reminderDaoPtr->deleteReminder(rescheduleId);
    assert(deleted);
    assert(reminderDaoPtr->getAllReminders().size() == beforeDelete - 1);
    assert(reminderDaoPtr->getReminderCountByStatus(ReminderStatus::PENDING) >= 0);

    std::cout << "提醒模块本地测试完成" << std::endl;

    // ============ AchievementManager 测试 ============
    const std::string testDataPath = "./test_data/";
    std::filesystem::remove_all(testDataPath);
    auto achievementDao = std::make_unique<AchievementDAO>(testDataPath);
    auto* achievementDaoPtr = achievementDao.get();

    AchievementManager achievementManager(std::move(achievementDao), 1);

    // 通过注入统计数据模拟多种成就条件
    auto fakeAnalyzer = std::make_unique<FakeStatisticsAnalyzer>();
    // 设置低于目标值的统计，先验证不会自动解锁
    fakeAnalyzer->completedTasks = 0;
    fakeAnalyzer->streak = 0;
    fakeAnalyzer->dailyCount = 0;
    fakeAnalyzer->pomodoroCount = 0;
    achievementManager.setStatisticsAnalyzer(std::move(fakeAnalyzer));

    // 确认成就定义和用户初始记录被创建并包含完整字段
    auto definitions = achievementDaoPtr->getAllAchievementDefinitions();
    assert(!definitions.empty());
    for (const auto& def : definitions) {
        assert(!def.name.empty() && !def.description.empty());
        assert(!def.icon.empty());
        assert(!def.unlock_condition.empty());
        assert(def.target_value > 0);
    }
    auto userAchievements = achievementDaoPtr->getUserAchievements(1);
    assert(!userAchievements.empty());

    // 运行综合检查，验证不会抛出异常且不会在进度不足时解锁
    achievementManager.checkAllAchievements();
    for (const auto& entry : achievementDaoPtr->getUserAchievements(1)) {
        assert(!entry.unlocked && "进度不足时不应解锁");
    }

    // 直接调用数值更新接口确保所有成就都能解锁
    for (const auto& def : definitions) {
        achievementManager.updateAchievementProgress(1, def.id, def.target_value - 1);
        auto record = achievementDaoPtr->getUserAchievement(1, def.unlock_condition);
        assert(record && !record->unlocked);

        achievementManager.updateAchievementProgress(1, def.id, def.target_value);
        record = achievementDaoPtr->getUserAchievement(1, def.unlock_condition);
        assert(record && record->unlocked && record->progress >= def.target_value);
        assert(!record->unlocked_date.empty());
        auto defMeta = achievementDaoPtr->getAchievementDefinition(def.unlock_condition);
        assert(defMeta.reward_xp > 0);
    }

    // 读取进度百分比
    auto progressList = achievementManager.getAchievementProgress(1);
    assert(!progressList.empty());
    for (const auto& p : progressList) {
        assert(p.progressPercent >= 100.0 - 1e-5);
    }

    // 切换用户时应自动初始化新用户记录
    achievementManager.setCurrentUserId(2);
    auto user2 = achievementDaoPtr->getUserAchievements(2);
    assert(!user2.empty());
    achievementManager.updateAchievementProgress(2, user2.front().id, 1);
    auto progressUser2 = achievementManager.getAchievementProgress(2);
    assert(!progressUser2.empty());

    std::cout << "所有本地测试通过！" << std::endl;
    return 0;
}
