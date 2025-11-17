# 成员D任务修改方案
# Member D (Yu Zhixuan) - Complete Task Plan

**生成日期**: 2025-11-17  
**负责人**: Yu Zhixuan (成员D)  
**角色**: 数据库层负责人、DAO维护者  

---

## 📋 任务总览

成员D需要完成的任务分为**3个优先级**：

- 🔴 **紧急且阻塞** (2-3小时) - 必须立即完成
- 🟡 **高优先级** (3-4小时) - 尽快完成
- 🟢 **中优先级** (1-2小时) - 后续完成

**总预计时间**: **6-9小时** (1-1.5个工作日)

---

## 🔴 任务一：修复Include路径错误 (紧急阻塞)

**优先级**: 🔴 **最高** - 导致编译失败  
**预计时间**: 30分钟  
**影响范围**: 5个DAO文件

### 问题描述

多个DAO头文件中的include路径错误：
1. 使用了错误的文件名: `"Entities.h"` (应该是小写 `entities.h`)
2. 使用了错误的路径: `"common/Entities.h"` 或 `"commom/Entities.h"` (拼写错误)
3. 路径不正确，应该使用相对路径

### 需要修改的文件

#### 1. `include/database/DAO/ExperienceDAO.h`

**当前代码** (Line 4):
```cpp
#include "common/Entities.h"  // ❌ 错误
```

**修改为**:
```cpp
#include "../../common/entities.h"  // ✅ 正确
```

---

#### 2. `include/database/DAO/StatisticsDAO.h`

**当前代码** (Line 4):
```cpp
#include "common/Entities.h"  // ❌ 错误
```

**修改为**:
```cpp
#include "../../common/entities.h"  // ✅ 正确
```

---

#### 3. `include/database/DAO/TaskDAO.h`

**当前代码** (Line 4):
```cpp
#include "common/Entities.h"  // ❌ 错误
```

**修改为**:
```cpp
#include "../../common/entities.h"  // ✅ 正确
```

---

#### 4. `include/database/DAO/ReminderDAO.h`

**当前代码** (Line 4):
```cpp
#include "commom/Entities.h"  // ❌ 错误（拼写错误：commom）
```

**修改为**:
```cpp
#include "../../common/entities.h"  // ✅ 正确
```

---

#### 5. `include/database/DAO/SettingsDAO.h`

**当前代码** (Line 4):
```cpp
#include "commom/Entities.h"  // ❌ 错误（拼写错误：commom）
```

**修改为**:
```cpp
#include "../../common/entities.h"  // ✅ 正确
```

---

### 验证方法

修改完成后，运行以下命令验证：

```bash
# 检查是否还有错误的include
grep -r "Entities.h\|commom/" include/database/DAO/*.h

# 应该没有任何输出，如果有，说明还有文件没修复
```

---

## 🔴 任务二：添加缺失的枚举类型定义 (紧急阻塞)

**优先级**: 🔴 **最高** - 多个模块依赖  
**预计时间**: 1小时  
**影响范围**: common/entities.h

### 问题描述

多个DAO接口使用了枚举类型，但这些类型在 `common/entities.h` 中**未定义**，导致编译错误。

### 需要添加的枚举类型

在 `common/entities.h` 文件中，在 `BaseEntity` 定义之后、第一个实体结构体之前添加：

```cpp
// 在 common/entities.h 中，在 BaseEntity 之后添加：

/**
 * @brief 任务状态枚举
 * 
 * 用于TaskDAO和任务管理系统
 */
enum class TaskStatus {
    PENDING = 0,        // 待处理
    IN_PROGRESS = 1,    // 进行中
    COMPLETED = 2,      // 已完成
    CANCELLED = 3,      // 已取消
    BLOCKED = 4         // 被阻塞
};

/**
 * @brief 优先级枚举
 * 
 * 用于任务和项目的优先级标识
 */
enum class Priority {
    LOW = 0,           // 低优先级
    MEDIUM = 1,        // 中优先级
    HIGH = 2,          // 高优先级
    URGENT = 3         // 紧急
};

/**
 * @brief 提醒类型枚举
 * 
 * 负责人: Fei Yifan (提醒系统)
 */
enum class ReminderType {
    ONCE = 0,          // 一次性提醒
    DAILY = 1,         // 每日提醒
    WEEKLY = 2,        // 每周提醒
    MONTHLY = 3        // 每月提醒
};

/**
 * @brief 提醒状态枚举
 * 
 * 负责人: Fei Yifan (提醒系统)
 */
enum class ReminderStatus {
    PENDING = 0,       // 待处理
    TRIGGERED = 1,     // 已触发
    COMPLETED = 2,     // 已完成
    CANCELLED = 3,     // 已取消
    EXPIRED = 4        // 已过期
};
```

### 插入位置

在 `common/entities.h` 中的插入位置：

```cpp
#ifndef ENTITIES_H
#define ENTITIES_H

#include <string>
#include <vector>

/**
 * @brief 基础实体类，包含所有实体的通用字段
 */
struct BaseEntity {
    int id = 0;
    std::string created_date;
    std::string updated_date;
    
    BaseEntity() = default;
    virtual ~BaseEntity() = default;
};

// ========================================
// ✅ 在这里插入所有枚举定义
// ========================================

enum class TaskStatus {
    // ... (如上所示)
};

enum class Priority {
    // ... (如上所示)
};

enum class ReminderType {
    // ... (如上所示)
};

enum class ReminderStatus {
    // ... (如上所示)
};

// ========================================
// 实体定义开始
// ========================================

/**
 * @brief 任务实体 - 核心业务对象
 * 
 * 负责人: Kuang Wenqing (任务管理模块)
 */
struct Task : BaseEntity {
    // ... (现有代码)
};

// ... 其他实体定义
```

### 验证方法

```bash
# 编译测试
cd /path/to/project
make clean
make

# 应该没有关于TaskStatus, Priority, ReminderType, ReminderStatus未定义的错误
```

---

## 🔴 任务三：修改Reminder结构体以匹配DAO (紧急)

**优先级**: 🔴 **高** - 提醒系统无法编译  
**预计时间**: 30分钟  
**影响范围**: common/entities.h 中的 Reminder 结构体

### 问题描述

当前的 `Reminder` 结构体使用字符串和bool，但 `ReminderDAO` 期望使用枚举类型。

### 当前定义 (Line 91-100)

```cpp
struct Reminder : BaseEntity {
    std::string title;                   // 提醒标题
    std::string message;                 // 提醒消息
    std::string trigger_time;            // 触发时间
    std::string recurrence;              // ❌ 应该使用枚举
    bool triggered = false;              // ❌ 应该使用枚举
    int task_id = 0;                     // 关联的任务ID
    bool enabled = true;                 // 是否启用
    std::string last_triggered;          // 上次触发时间
    
    Reminder() = default;
    Reminder(const std::string& t, const std::string& msg, const std::string& time) 
        : title(t), message(msg), trigger_time(time) {}
};
```

### 修改为

```cpp
struct Reminder : BaseEntity {
    std::string title;                                 // 提醒标题
    std::string message;                               // 提醒消息
    std::string trigger_time;                          // 触发时间（ISO8601格式）
    ReminderType reminder_type = ReminderType::ONCE;   // ✅ 使用枚举
    ReminderStatus status = ReminderStatus::PENDING;   // ✅ 使用枚举
    int task_id = 0;                                   // 关联的任务ID
    std::string recurrence_rule = "";                  // 重复规则详细描述
    
    Reminder() = default;
    Reminder(const std::string& t, const std::string& msg, const std::string& time) 
        : title(t), message(msg), trigger_time(time) {}
};
```

### 说明

**删除的字段**:
- `std::string recurrence` - 替换为 `ReminderType reminder_type`
- `bool triggered` - 替换为 `ReminderStatus status`
- `bool enabled` - 可通过status判断
- `std::string last_triggered` - 移除（可在数据库层维护）

**新增/修改的字段**:
- `ReminderType reminder_type` - 提醒类型枚举
- `ReminderStatus status` - 提醒状态枚举
- `std::string recurrence_rule` - 详细的重复规则描述

---

## 🟡 任务四：实现ExperienceDAO (高优先级)

**优先级**: 🟡 **高** - XPSystem依赖  
**预计时间**: 2-3小时  
**影响范围**: src/database/DAO/ExperienceDAO.cpp

### 问题描述

`src/database/DAO/ExperienceDAO.cpp` 当前只有1行，完全未实现。

### 需要实现的接口

参考 `include/database/DAO/ExperienceDAO.h`，需要实现：

```cpp
class ExperienceDAO {
public:
    // 基础经验值操作
    virtual bool addExperience(int userId, int amount, const std::string& source) = 0;
    virtual int getTotalXP(int userId) = 0;
    virtual int getCurrentLevel(int userId) = 0;
    
    // 等级管理
    virtual UserLevelInfo getUserLevelInfo(int userId) = 0;
    virtual std::vector<LevelDefinition> getAllLevelDefinitions() = 0;
    virtual bool updateUserLevel(int userId, int level, int totalXP) = 0;
    
    // 经验值记录
    virtual std::vector<ExperienceRecord> getExperienceHistory(int userId, int limit = 50) = 0;
    virtual std::vector<ExperienceRecord> getExperienceBySource(int userId, const std::string& source) = 0;
    virtual std::vector<ExperienceRecord> getExperienceByDateRange(
        int userId, 
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) = 0;
    
    // 排行榜
    virtual std::vector<UserRanking> getTopUsersByXP(int limit = 10) = 0;
    virtual int getUserRank(int userId) = 0;
    
    // 统计查询
    virtual int getTotalXPBySource(int userId, const std::string& source) = 0;
    virtual int getXPEarnedToday(int userId) = 0;
    virtual int getXPEarnedThisWeek(int userId) = 0;
    virtual std::map<std::string, int> getXPBreakdownBySource(int userId) = 0;
};
```

### 实现建议

**步骤1**: 创建实现类

```cpp
// src/database/DAO/ExperienceDAO.cpp

#include "ExperienceDAO.h"
#include <sqlite3.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

class SQLiteExperienceDAO : public ExperienceDAO {
private:
    sqlite3* db;
    std::string dbPath;
    
    // 辅助方法
    std::string getCurrentDateTime() {
        // 实现时间格式化
    }
    
public:
    SQLiteExperienceDAO(const std::string& path) : dbPath(path), db(nullptr) {
        // 打开数据库连接
        int result = sqlite3_open(dbPath.c_str(), &db);
        if (result != SQLITE_OK) {
            std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        }
    }
    
    ~SQLiteExperienceDAO() {
        if (db) {
            sqlite3_close(db);
        }
    }
    
    // 实现所有接口方法
    bool addExperience(int userId, int amount, const std::string& source) override {
        // 实现插入经验值记录
        const char* sql = "INSERT INTO experience_records (user_id, amount, source, timestamp) VALUES (?, ?, ?, ?);";
        // ... 使用prepared statement
    }
    
    // ... 实现其他方法
};
```

**步骤2**: 参考已完成的DAO实现

可以参考以下已完成的DAO作为模板：
- `src/database/DAO/TaskDAO.cpp` (804行) - 最完整的实现
- `src/database/DAO/ProjectDAO.cpp` (595行)
- `src/database/DAO/ReminderDAO.cpp` (519行)

**预计代码量**: 400-600行

---

## 🟡 任务五：实现StatisticsDAO (高优先级)

**优先级**: 🟡 **高** - StatisticsAnalyzer依赖  
**预计时间**: 2-3小时  
**影响范围**: src/database/DAO/StatisticsDAO.cpp

### 问题描述

`src/database/DAO/StatisticsDAO.cpp` 当前只有1行，完全未实现。

### 需要实现的接口

参考 `include/database/DAO/StatisticsDAO.h`，需要实现：

```cpp
class StatisticsDAO {
public:
    // 任务统计
    virtual int getTotalTasksCount() = 0;
    virtual int getCompletedTasksCount() = 0;
    virtual double getCompletionRate() = 0;
    
    // 时间统计
    virtual int getTasksCompletedToday() = 0;
    virtual int getTasksCompletedThisWeek() = 0;
    virtual int getTasksCompletedThisMonth() = 0;
    virtual std::vector<DailyCompletionStats> getDailyCompletionStats(int days = 30) = 0;
    
    // 生产力分析
    virtual double getAverageTasksPerDay(int days = 30) = 0;
    virtual ProductivityReport getProductivityReport(const std::string& startDate, const std::string& endDate) = 0;
    
    // 连续打卡
    virtual int getCurrentStreak() = 0;
    virtual int getLongestStreak() = 0;
    virtual std::vector<StreakRecord> getStreakHistory() = 0;
    
    // 番茄钟统计
    virtual PomodoroStatistics getPomodoroStatistics() = 0;
    virtual int getTotalPomodoros() = 0;
    virtual int getPomodorosToday() = 0;
    
    // 热力图数据
    virtual std::vector<HeatmapData> getHeatmapData(int days = 90) = 0;
    virtual HeatmapData getTaskCountForDate(const std::string& date) = 0;
    
    // 项目统计
    virtual int getTotalProjectsCount() = 0;
    virtual double getAverageProjectProgress() = 0;
    virtual int getCompletedProjectsCount() = 0;
};
```

### 实现建议

**步骤1**: 创建实现类

```cpp
// src/database/DAO/StatisticsDAO.cpp

#include "StatisticsDAO.h"
#include <sqlite3.h>
#include <iostream>
#include <sstream>

class SQLiteStatisticsDAO : public StatisticsDAO {
private:
    sqlite3* db;
    std::string dbPath;
    
public:
    SQLiteStatisticsDAO(const std::string& path) : dbPath(path), db(nullptr) {
        int result = sqlite3_open(dbPath.c_str(), &db);
        if (result != SQLITE_OK) {
            std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        }
    }
    
    ~SQLiteStatisticsDAO() {
        if (db) {
            sqlite3_close(db);
        }
    }
    
    // 实现所有接口方法
    int getTotalTasksCount() override {
        const char* sql = "SELECT COUNT(*) FROM tasks;";
        // ... 执行查询
    }
    
    // ... 实现其他方法
};
```

**注意**: 
- 目前 StatisticsAnalyzer 已经直接使用SQLite C API实现了功能
- 这个DAO是为了提供标准化的数据访问层
- 实现后可以选择性地重构StatisticsAnalyzer使用DAO

**预计代码量**: 400-600行

---

## 🟢 任务六：实现SettingsDAO (中优先级)

**优先级**: 🟢 **中** - 可选功能  
**预计时间**: 1-2小时  
**影响范围**: src/database/DAO/SettingsDAO.cpp

### 问题描述

`src/database/DAO/SettingsDAO.cpp` 当前只有1行，完全未实现。

### 需要实现的接口

参考 `include/database/DAO/SettingsDAO.h`，需要实现用户设置的CRUD操作。

### 实现建议

这是优先级较低的任务，可以在完成前5个任务后再处理。

**预计代码量**: 200-300行

---

## 🟢 任务七：实现DAOFactory (中优先级)

**优先级**: 🟢 **中** - 架构完善  
**预计时间**: 30分钟  
**影响范围**: src/database/DAO/DAOFactory.cpp

### 问题描述

`src/database/DAO/DAOFactory.cpp` 当前只有1行，完全未实现。

### 实现建议

创建工厂类，统一管理所有DAO实例的创建：

```cpp
// src/database/DAO/DAOFactory.cpp

#include "DAOFactory.h"
#include "TaskDAO.h"
#include "ProjectDAO.h"
#include "ReminderDAO.h"
#include "ExperienceDAO.h"
#include "StatisticsDAO.h"
#include "AchievementDAO.h"
#include "SettingsDAO.h"

class SQLiteDAOFactory : public DAOFactory {
private:
    std::string dbPath;
    
public:
    SQLiteDAOFactory(const std::string& path) : dbPath(path) {}
    
    std::unique_ptr<TaskDAO> createTaskDAO() override {
        return std::make_unique<SQLiteTaskDAO>(dbPath);
    }
    
    std::unique_ptr<ProjectDAO> createProjectDAO() override {
        return std::make_unique<SQLiteProjectDAO>(dbPath);
    }
    
    // ... 其他DAO创建方法
};
```

**预计代码量**: 50-100行

---

## 📊 任务优先级和时间表

### 立即执行 (Day 1 上午，2-3小时)

**必须按顺序完成，否则无法编译！**

1. ✅ **任务一**: 修复Include路径 (30分钟)
2. ✅ **任务二**: 添加枚举定义 (1小时)
3. ✅ **任务三**: 修改Reminder结构体 (30分钟)

**验证**: 编译测试
```bash
make clean
make
```

---

### 尽快完成 (Day 1 下午 + Day 2，6小时)

4. ✅ **任务四**: 实现ExperienceDAO (2-3小时)
5. ✅ **任务五**: 实现StatisticsDAO (2-3小时)

**验证**: 运行相关模块测试

---

### 后续完成 (Day 3，2小时)

6. ✅ **任务六**: 实现SettingsDAO (1-2小时)
7. ✅ **任务七**: 实现DAOFactory (30分钟)

---

## 📝 详细执行步骤

### 第一步：修复Include路径 (30分钟)

```bash
# 1. 打开每个文件
vim include/database/DAO/ExperienceDAO.h
vim include/database/DAO/StatisticsDAO.h
vim include/database/DAO/TaskDAO.h
vim include/database/DAO/ReminderDAO.h
vim include/database/DAO/SettingsDAO.h

# 2. 将第4行的include路径全部改为：
#include "../../common/entities.h"

# 3. 验证
grep -r "Entities.h\|commom/" include/database/DAO/*.h
# 应该没有任何输出
```

---

### 第二步：添加枚举定义 (1小时)

```bash
# 1. 打开文件
vim common/entities.h

# 2. 在BaseEntity定义之后、Task结构体之前，插入所有枚举定义
# （参考上面"任务二"的完整代码）

# 3. 保存并验证编译
make clean
make
```

---

### 第三步：修改Reminder结构体 (30分钟)

```bash
# 1. 打开文件
vim common/entities.h

# 2. 找到Reminder结构体（大约Line 91）

# 3. 替换为新的定义（参考上面"任务三"的代码）

# 4. 保存并验证编译
make clean
make
```

---

### 第四步：实现ExperienceDAO (2-3小时)

```bash
# 1. 打开文件
vim src/database/DAO/ExperienceDAO.cpp

# 2. 参考TaskDAO.cpp的实现结构

# 3. 实现所有接口方法

# 4. 编译测试
make clean
make
```

---

### 第五步：实现StatisticsDAO (2-3小时)

```bash
# 1. 打开文件
vim src/database/DAO/StatisticsDAO.cpp

# 2. 参考TaskDAO.cpp的实现结构

# 3. 实现所有接口方法

# 4. 编译测试
make clean
make
```

---

## 🎯 完成标准

### 验证清单

完成所有任务后，确认以下事项：

- [ ] **编译成功**: `make clean && make` 无错误
- [ ] **Include路径正确**: 无 "Entities.h" 或 "commom/" 引用
- [ ] **枚举定义存在**: TaskStatus, Priority, ReminderType, ReminderStatus 可用
- [ ] **Reminder结构体匹配DAO**: 使用枚举而非字符串
- [ ] **ExperienceDAO实现完整**: 所有接口方法实现
- [ ] **StatisticsDAO实现完整**: 所有接口方法实现
- [ ] **代码风格一致**: 与现有DAO保持一致
- [ ] **注释完整**: 关键方法有注释说明

---

## 📞 协作与沟通

### 完成任务一至三后通知团队

发送消息给：
- **成员E (Mao Jingqi)**: 可以开始集成ReminderSystem了
- **成员A (Fei Yifan)**: 类型定义已修复，可以开始测试
- **成员C (Kuang Wenqing)**: 类型定义已就绪，TaskManager可以使用

### 完成任务四后通知团队

发送消息给：
- **成员E**: ExperienceDAO已完成，XPSystem可以选择使用DAO重构

### 完成任务五后通知团队

发送消息给：
- **成员E**: StatisticsDAO已完成，StatisticsAnalyzer可以选择使用DAO重构

---

## 🎉 总结

### 任务重要性排序

**最重要** 🔴:
1. 修复Include路径 - **阻塞编译**
2. 添加枚举定义 - **阻塞编译**
3. 修改Reminder结构体 - **阻塞提醒系统**

**很重要** 🟡:
4. 实现ExperienceDAO - **游戏化系统完善**
5. 实现StatisticsDAO - **统计系统标准化**

**重要** 🟢:
6. 实现SettingsDAO - **功能完善**
7. 实现DAOFactory - **架构完善**

### 时间分配

- **Day 1 上午**: 完成任务1-3（必须！）
- **Day 1 下午**: 开始任务4
- **Day 2**: 完成任务4-5
- **Day 3**: 完成任务6-7（可选）

### 预期结果

完成后：
- ✅ 项目可以成功编译
- ✅ 提醒系统可以集成
- ✅ XP系统有完整的数据访问层
- ✅ 统计系统有标准化的DAO
- ✅ 项目完成度提升到 **80%+**

---

**加油！你的工作对项目至关重要！** 💪

---

**文档完成** ✅
