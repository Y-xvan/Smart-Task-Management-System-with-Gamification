#include "database/DAO/ReminderDAO.h"
#include "database/DatabaseManager.h"
#include <sqlite3.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>

// 定义 SELECT 列常量（用于复用）
#define SELECT_COLUMNS_STR "id, title, message, trigger_time, recurrence, triggered, task_id, enabled, last_triggered "

class SQLiteReminderDAO : public ReminderDAO {
private:
    DatabaseManager& dbManager;
    std::string dbPath;

    // SQL 语句常量（与 DatabaseManager::createReminderTable 保持一致）
    static constexpr const char* INSERT_REMINDER_SQL =
        "INSERT INTO reminders (title, message, trigger_time, recurrence, triggered, task_id, enabled, last_triggered) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";

    static constexpr const char* UPDATE_REMINDER_SQL =
        "UPDATE reminders SET title=?, message=?, trigger_time=?, recurrence=?, triggered=?, task_id=?, enabled=?, "
        "last_triggered=?, updated_date=datetime('now') WHERE id=?;";

    static constexpr const char* DELETE_REMINDER_SQL =
        "DELETE FROM reminders WHERE id=?;";

    static constexpr const char* SELECT_BY_ID_SQL =
        "SELECT " SELECT_COLUMNS_STR "FROM reminders WHERE id=?;";

    static constexpr const char* SELECT_ALL_SQL =
        "SELECT " SELECT_COLUMNS_STR "FROM reminders ORDER BY trigger_time ASC;";

    static constexpr const char* SELECT_ACTIVE_SQL =
        "SELECT " SELECT_COLUMNS_STR "FROM reminders WHERE enabled = 1 AND triggered = 0 ORDER BY trigger_time ASC;";

    static constexpr const char* SELECT_BY_TASK_SQL =
        "SELECT " SELECT_COLUMNS_STR "FROM reminders WHERE task_id=? AND enabled = 1 ORDER BY trigger_time ASC;";

    static constexpr const char* SELECT_BY_RECURRENCE_SQL =
        "SELECT " SELECT_COLUMNS_STR "FROM reminders WHERE recurrence=? AND enabled = 1 ORDER BY trigger_time ASC;";

    static constexpr const char* SELECT_DUE_SQL =
        "SELECT " SELECT_COLUMNS_STR "FROM reminders WHERE trigger_time <= ? AND enabled = 1 AND triggered = 0 ORDER BY trigger_time ASC;";

    static constexpr const char* SELECT_DUE_TODAY_SQL =
        "SELECT " SELECT_COLUMNS_STR "FROM reminders WHERE date(trigger_time) = date('now') AND enabled = 1 AND triggered = 0 "
        "ORDER BY trigger_time ASC;";

    static constexpr const char* SELECT_DUE_WEEK_SQL =
        "SELECT " SELECT_COLUMNS_STR "FROM reminders WHERE date(trigger_time) BETWEEN date('now') AND date('now', '+6 days') "
        "AND enabled = 1 AND triggered = 0 ORDER BY trigger_time ASC;";

    static constexpr const char* SELECT_BY_DATE_RANGE_SQL =
        "SELECT " SELECT_COLUMNS_STR "FROM reminders WHERE trigger_time BETWEEN ? AND ? AND enabled = 1 ORDER BY trigger_time ASC;";

    static constexpr const char* MARK_TRIGGERED_SQL =
        "UPDATE reminders SET triggered = 1, last_triggered = datetime('now'), updated_date = datetime('now') WHERE id=?;";

    static constexpr const char* MARK_COMPLETED_SQL =
        "UPDATE reminders SET triggered = 1, enabled = 0, last_triggered = datetime('now'), updated_date = datetime('now') WHERE id=?;";

    static constexpr const char* RESCHEDULE_SQL =
        "UPDATE reminders SET trigger_time=?, triggered = 0, enabled = 1, updated_date = datetime('now') WHERE id=?;";

    static constexpr const char* SELECT_RECURRING_SQL =
        "SELECT " SELECT_COLUMNS_STR "FROM reminders WHERE recurrence != 'once' AND enabled = 1 ORDER BY trigger_time ASC;";

    static constexpr const char* DELETE_EXPIRED_SQL =
        "DELETE FROM reminders WHERE triggered = 1 AND last_triggered < datetime('now', '-30 days');";

    static constexpr const char* CLEANUP_COMPLETED_SQL =
        "DELETE FROM reminders WHERE triggered = 1 AND enabled = 0;";

    static constexpr const char* COUNT_PENDING_SQL =
        "SELECT COUNT(*) FROM reminders WHERE enabled = 1 AND triggered = 0;";

    static constexpr const char* COUNT_TRIGGERED_SQL =
        "SELECT COUNT(*) FROM reminders WHERE triggered = 1;";

    static constexpr const char* COUNT_CANCELLED_SQL =
        "SELECT COUNT(*) FROM reminders WHERE enabled = 0;";

    static constexpr const char* COUNT_OVERDUE_SQL =
        "SELECT COUNT(*) FROM reminders WHERE trigger_time < datetime('now') AND enabled = 1 AND triggered = 0;";

    sqlite3* getDb() {
        if (!dbManager.isOpen()) {
            dbManager.initialize(dbPath.empty() ? "task_manager.db" : dbPath);
        }
        return dbManager.getRawConnection();
    }

    bool ensureTable() {
        return dbManager.createTables();
    }

    static std::string timePointToString(const std::chrono::system_clock::time_point& tp) {
        auto time_t = std::chrono::system_clock::to_time_t(tp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    static std::chrono::system_clock::time_point stringToTimePoint(const std::string& timeStr) {
        std::tm tm = {};
        std::stringstream ss(timeStr);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        auto time_t = std::mktime(&tm);
        return std::chrono::system_clock::from_time_t(time_t);
    }

    static ReminderType toReminderType(const std::string& recurrence) {
        if (recurrence == "daily") return ReminderType::DAILY;
        if (recurrence == "weekly") return ReminderType::WEEKLY;
        if (recurrence == "monthly") return ReminderType::MONTHLY;
        return ReminderType::ONCE;
    }

    std::optional<Reminder> extractReminderFromStatement(sqlite3_stmt* stmt) {
        Reminder reminder;
        reminder.id = sqlite3_column_int(stmt, 0);
        reminder.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        reminder.message = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

        const std::string triggerTimeStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        reminder.trigger_time = triggerTimeStr;
        reminder.triggerTime = stringToTimePoint(triggerTimeStr);

        reminder.recurrence = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        reminder.recurrenceRule = reminder.recurrence;
        reminder.type = toReminderType(reminder.recurrence);

        reminder.triggered = sqlite3_column_int(stmt, 5) != 0;
        reminder.status = reminder.triggered ? ReminderStatus::TRIGGERED : ReminderStatus::PENDING;

        reminder.task_id = sqlite3_column_int(stmt, 6);
        reminder.taskId = reminder.task_id;

        reminder.enabled = sqlite3_column_int(stmt, 7) != 0;

        const unsigned char* lastTriggered = sqlite3_column_text(stmt, 8);
        if (lastTriggered) {
            reminder.last_triggered = reinterpret_cast<const char*>(lastTriggered);
        }

        return reminder;
    }

    std::vector<Reminder> extractRemindersFromStatement(sqlite3_stmt* stmt) {
        std::vector<Reminder> reminders;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            auto reminder = extractReminderFromStatement(stmt);
            if (reminder) {
                reminders.push_back(*reminder);
            }
        }

        return reminders;
    }

    int executeCountQuery(const char* sql) {
        sqlite3* db = getDb();
        if (!db) return -1;

        sqlite3_stmt* stmt = nullptr;
        int count = -1;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                count = sqlite3_column_int(stmt, 0);
            }
            sqlite3_finalize(stmt);
        }

        return count;
    }

    std::chrono::system_clock::time_point calculateNextTrigger(const Reminder& reminder) {
        auto nextTime = reminder.triggerTime;
        if (reminder.recurrence == "daily") {
            nextTime += std::chrono::hours(24);
        } else if (reminder.recurrence == "weekly") {
            nextTime += std::chrono::hours(24 * 7);
        } else if (reminder.recurrence == "monthly") {
            nextTime += std::chrono::hours(24 * 30);
        }
        return nextTime;
    }

public:
    explicit SQLiteReminderDAO(const std::string& databasePath = "task_manager.db")
        : dbManager(DatabaseManager::getInstance()), dbPath(databasePath) {
        getDb();
        ensureTable();
    }

    ~SQLiteReminderDAO() override = default;

    // 基础CRUD操作
    bool insertReminder(Reminder& reminder) override {
        sqlite3* db = getDb();
        if (!db) {
            std::cerr << "[DEBUG] insertReminder: db is null" << std::endl;
            return false;
        }

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, INSERT_REMINDER_SQL, -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "准备插入语句失败: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        const std::string triggerTimeStr = timePointToString(reminder.triggerTime);

        sqlite3_bind_text(stmt, 1, reminder.title.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, reminder.message.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, triggerTimeStr.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, reminder.recurrence.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 5, reminder.triggered ? 1 : 0);
        // Handle task_id: bind NULL if taskId is 0 (no task linked)
        if (reminder.taskId > 0) {
            sqlite3_bind_int(stmt, 6, reminder.taskId);
        } else {
            sqlite3_bind_null(stmt, 6);
        }
        sqlite3_bind_int(stmt, 7, reminder.enabled ? 1 : 0);
        sqlite3_bind_text(stmt, 8, reminder.last_triggered.c_str(), -1, SQLITE_TRANSIENT);

        int stepResult = sqlite3_step(stmt);
        const bool success = (stepResult == SQLITE_DONE);
        if (!success) {
            std::cerr << "insertReminder: sqlite3_step failed with code " << stepResult << ": " << sqlite3_errmsg(db) << std::endl;
        }
        if (success) {
            reminder.id = static_cast<int>(sqlite3_last_insert_rowid(db));
        }

        sqlite3_finalize(stmt);
        return success;
    }

    bool updateReminder(const Reminder& reminder) override {
        sqlite3* db = getDb();
        if (!db) return false;

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, UPDATE_REMINDER_SQL, -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "准备更新语句失败: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        const std::string triggerTimeStr = timePointToString(reminder.triggerTime);

        sqlite3_bind_text(stmt, 1, reminder.title.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, reminder.message.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, triggerTimeStr.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, reminder.recurrence.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 5, reminder.triggered ? 1 : 0);
        // Handle task_id: bind NULL if taskId is 0 (no task linked)
        if (reminder.taskId > 0) {
            sqlite3_bind_int(stmt, 6, reminder.taskId);
        } else {
            sqlite3_bind_null(stmt, 6);
        }
        sqlite3_bind_int(stmt, 7, reminder.enabled ? 1 : 0);
        sqlite3_bind_text(stmt, 8, reminder.last_triggered.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 9, reminder.id);

        const bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    bool deleteReminder(int reminderId) override {
        sqlite3* db = getDb();
        if (!db) return false;

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, DELETE_REMINDER_SQL, -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "准备删除语句失败: " << sqlite3_errmsg(db) << std::endl;
            return false;
        }

        sqlite3_bind_int(stmt, 1, reminderId);
        const bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    // 查询操作
    std::optional<Reminder> getReminderById(int reminderId) override {
        sqlite3* db = getDb();
        if (!db) return std::nullopt;

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, SELECT_BY_ID_SQL, -1, &stmt, nullptr) != SQLITE_OK) {
            return std::nullopt;
        }

        sqlite3_bind_int(stmt, 1, reminderId);

        std::optional<Reminder> reminder = std::nullopt;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            reminder = extractReminderFromStatement(stmt);
        }

        sqlite3_finalize(stmt);
        return reminder;
    }

    std::vector<Reminder> getAllReminders() override {
        sqlite3* db = getDb();
        if (!db) return {};

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, SELECT_ALL_SQL, -1, &stmt, nullptr) != SQLITE_OK) {
            return {};
        }

        auto reminders = extractRemindersFromStatement(stmt);
        sqlite3_finalize(stmt);
        return reminders;
    }

    std::vector<Reminder> getActiveReminders() override {
        sqlite3* db = getDb();
        if (!db) return {};

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, SELECT_ACTIVE_SQL, -1, &stmt, nullptr) != SQLITE_OK) {
            return {};
        }

        auto reminders = extractRemindersFromStatement(stmt);
        sqlite3_finalize(stmt);
        return reminders;
    }

    std::vector<Reminder> getRemindersByTask(int taskId) override {
        sqlite3* db = getDb();
        if (!db) return {};

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, SELECT_BY_TASK_SQL, -1, &stmt, nullptr) != SQLITE_OK) {
            return {};
        }

        sqlite3_bind_int(stmt, 1, taskId);
        auto reminders = extractRemindersFromStatement(stmt);
        sqlite3_finalize(stmt);
        return reminders;
    }

    std::vector<Reminder> getRemindersByType(ReminderType type) override {
        switch (type) {
        case ReminderType::DAILY:
            return getRemindersByRecurrence("daily");
        case ReminderType::WEEKLY:
            return getRemindersByRecurrence("weekly");
        case ReminderType::MONTHLY:
            return getRemindersByRecurrence("monthly");
        case ReminderType::ONCE:
        default:
            return getRemindersByRecurrence("once");
        }
    }

    std::vector<Reminder> getRemindersByRecurrence(const std::string& recurrence) override {
        sqlite3* db = getDb();
        if (!db) return {};

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, SELECT_BY_RECURRENCE_SQL, -1, &stmt, nullptr) != SQLITE_OK) {
            return {};
        }

        sqlite3_bind_text(stmt, 1, recurrence.c_str(), -1, SQLITE_TRANSIENT);
        auto reminders = extractRemindersFromStatement(stmt);
        sqlite3_finalize(stmt);
        return reminders;
    }

    std::vector<Reminder> getDueReminders(const std::chrono::system_clock::time_point& currentTime) override {
        sqlite3* db = getDb();
        if (!db) return {};

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, SELECT_DUE_SQL, -1, &stmt, nullptr) != SQLITE_OK) {
            return {};
        }

        const std::string currentTimeStr = timePointToString(currentTime);
        sqlite3_bind_text(stmt, 1, currentTimeStr.c_str(), -1, SQLITE_TRANSIENT);

        auto reminders = extractRemindersFromStatement(stmt);
        sqlite3_finalize(stmt);
        return reminders;
    }

    // 时间相关查询
    std::vector<Reminder> getRemindersDueToday() override {
        sqlite3* db = getDb();
        if (!db) return {};

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, SELECT_DUE_TODAY_SQL, -1, &stmt, nullptr) != SQLITE_OK) {
            return {};
        }

        auto reminders = extractRemindersFromStatement(stmt);
        sqlite3_finalize(stmt);
        return reminders;
    }

    std::vector<Reminder> getRemindersDueThisWeek() override {
        sqlite3* db = getDb();
        if (!db) return {};

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, SELECT_DUE_WEEK_SQL, -1, &stmt, nullptr) != SQLITE_OK) {
            return {};
        }

        auto reminders = extractRemindersFromStatement(stmt);
        sqlite3_finalize(stmt);
        return reminders;
    }

    std::vector<Reminder> getRemindersByDateRange(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) override {
        sqlite3* db = getDb();
        if (!db) return {};

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, SELECT_BY_DATE_RANGE_SQL, -1, &stmt, nullptr) != SQLITE_OK) {
            return {};
        }

        const std::string startStr = timePointToString(start);
        const std::string endStr = timePointToString(end);

        sqlite3_bind_text(stmt, 1, startStr.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, endStr.c_str(), -1, SQLITE_TRANSIENT);

        auto reminders = extractRemindersFromStatement(stmt);
        sqlite3_finalize(stmt);
        return reminders;
    }

    // 状态管理
    bool markReminderAsTriggered(int reminderId) override {
        sqlite3* db = getDb();
        if (!db) return false;

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, MARK_TRIGGERED_SQL, -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }

        sqlite3_bind_int(stmt, 1, reminderId);
        const bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    bool markReminderAsCompleted(int reminderId) override {
        sqlite3* db = getDb();
        if (!db) return false;

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, MARK_COMPLETED_SQL, -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }

        sqlite3_bind_int(stmt, 1, reminderId);
        const bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    bool rescheduleReminder(int reminderId, const std::chrono::system_clock::time_point& newTime) override {
        sqlite3* db = getDb();
        if (!db) return false;

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, RESCHEDULE_SQL, -1, &stmt, nullptr) != SQLITE_OK) {
            return false;
        }

        const std::string newTimeStr = timePointToString(newTime);
        sqlite3_bind_text(stmt, 1, newTimeStr.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, reminderId);

        const bool success = (sqlite3_step(stmt) == SQLITE_DONE);
        sqlite3_finalize(stmt);
        return success;
    }

    // 重复提醒
    bool createNextRecurringReminder(int originalReminderId) override {
        auto original = getReminderById(originalReminderId);
        if (!original || original->recurrence == "once") {
            return false;
        }

        Reminder newReminder = *original;
        newReminder.id = 0;
        newReminder.triggered = false;
        newReminder.status = ReminderStatus::PENDING;
        newReminder.triggerTime = calculateNextTrigger(*original);
        newReminder.trigger_time = timePointToString(newReminder.triggerTime);
        newReminder.last_triggered.clear();
        newReminder.enabled = true;

        return insertReminder(newReminder);
    }

    std::vector<Reminder> getRecurringReminders() override {
        sqlite3* db = getDb();
        if (!db) return {};

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, SELECT_RECURRING_SQL, -1, &stmt, nullptr) != SQLITE_OK) {
            return {};
        }

        auto reminders = extractRemindersFromStatement(stmt);
        sqlite3_finalize(stmt);
        return reminders;
    }

    // 清理与统计
    bool deleteExpiredReminders() override {
        sqlite3* db = getDb();
        if (!db) return false;

        return dbManager.execute(DELETE_EXPIRED_SQL);
    }

    bool cleanUpCompletedReminders() override {
        sqlite3* db = getDb();
        if (!db) return false;

        return dbManager.execute(CLEANUP_COMPLETED_SQL);
    }

    int getReminderCountByStatus(ReminderStatus status) override {
        switch (status) {
        case ReminderStatus::PENDING:
            return executeCountQuery(COUNT_PENDING_SQL);
        case ReminderStatus::TRIGGERED:
            return executeCountQuery(COUNT_TRIGGERED_SQL);
        case ReminderStatus::CANCELLED:
            return executeCountQuery(COUNT_CANCELLED_SQL);
        case ReminderStatus::COMPLETED:
            return executeCountQuery(COUNT_TRIGGERED_SQL);
        default:
            return -1;
        }
    }

    int getOverdueReminderCount() override {
        return executeCountQuery(COUNT_OVERDUE_SQL);
    }
};

// 工厂函数
std::unique_ptr<ReminderDAO> createReminderDAO(const std::string& dbPath) {
    auto dao = std::make_unique<SQLiteReminderDAO>(dbPath);
    return dao;
}