# 项目管理与热力图可视化模块

**负责人**: 周天健 (成员B)  
**代码量**: ~500行  
**完成时间**: Week 7-12

---

## 📋 模块概述

本模块负责游戏化任务管理系统的**项目管理**和**任务完成热力图可视化**功能。

### 核心功能
- ✅ 项目CRUD操作（创建、查询、更新、删除）
- ✅ 项目进度自动计算
- ✅ 任务按项目分组
- ✅ 90天任务完成热力图
- ✅ 月视图/周视图切换
- ✅ ASCII艺术渲染

---

## 📁 文件结构

```
cpp_project/
├── include/
│   ├── Project.h                 # 项目实体类
│   ├── ProjectManager.h          # 项目管理器
│   └── HeatmapVisualizer.h       # 热力图可视化
├── src/
│   ├── Project.cpp
│   ├── ProjectManager.cpp
│   └── HeatmapVisualizer.cpp
├── test_project.cpp              # 项目管理测试
├── test_heatmap.cpp              # 热力图测试
└── README.md
```

---

## 🏗️ 类设计

### 1. Project 类

**项目实体类**，包含项目的所有属性和基本操作。

```cpp
class Project {
private:
    int id;                    // 项目ID
    string name;               // 项目名称
    string description;        // 项目描述
    string color_label;        // 颜色标签
    double progress;           // 进度(0.0-1.0)
    int total_tasks;           // 总任务数
    int completed_tasks;       // 已完成任务数
    string target_date;        // 目标日期
    bool archived;             // 是否归档
    string created_date;       // 创建日期

public:
    // 构造函数
    Project();
    Project(string name, string desc, string color);
    
    // Getter/Setter
    int getId() const;
    string getName() const;
    // ... 其他方法
    
    // 业务方法
    void updateProgress();     // 更新进度
    bool isCompleted() const;  // 是否完成
};
```

---

### 2. ProjectManager 类

**项目管理器**，负责项目的业务逻辑处理。

```cpp
class ProjectManager {
private:
    map<int, Project*> projects;  // 项目存储（后续替换为数据库）
    int nextId;

public:
    // CRUD操作
    int createProject(const Project& project);
    Project* getProject(int id);
    vector<Project*> getAllProjects();
    bool updateProject(const Project& project);
    bool deleteProject(int id);
    
    // 业务功能
    double calculateProgress(int project_id);
    void updateProjectProgress(int project_id);
    int getProjectCount();
};
```

---

### 3. HeatmapVisualizer 类

**热力图可视化**，以ASCII艺术形式展示任务完成情况。

```cpp
class HeatmapVisualizer {
private:
    map<string, int> taskData;  // 日期->任务数映射
    
    string getColorBlock(int count);
    int getTaskCount(string date);

public:
    // 添加数据
    void addTaskData(string date, int count);
    
    // 可视化生成
    string generateHeatmap(int days = 90);
    string generateMonthView(string month);
    string generateWeekView(string startDate);
    
    // 统计信息
    int getTotalTasks();
    string getMostActiveDay();
    int getCurrentStreak();
};
```

---

## 🎨 热力图效果

```
═══════════════════════════════════════════════════
         📊 90天任务完成热力图
═══════════════════════════════════════════════════

      W1  W2  W3  W4  W5  W6  W7  W8  W9  W10 W11 W12
Mon   ░░  ░░  ▓▓  █   ░░  ▒▒  ▓▓  ░░  █   ▒▒  ░░  ▓▓
Tue   ▒▒  ░░  ▒▒  ▓▓  █   ░░  ▒▒  ▓▓  ░░  █   ▒▒  ░░
Wed   ▓▓  ▒▒  ░░  ▓▓  ▒▒  █   ░░  ▒▒  ▓▓  ░░  █   ▒▒
Thu   ░░  ▓▓  ▒▒  ░░  ▓▓  ▒▒  █   ░░  ▒▒  ▓▓  ░░  █
Fri   █   ░░  ▓▓  ▒▒  ░░  ▓▓  ▒▒  █   ░░  ▒▒  ▓▓  ░░
Sat   ▒▒  █   ░░  ▓▓  ▒▒  ░░  ▓▓  ▒▒  █   ░░  ▒▒  ▓▓
Sun   ▓▓  ▒▒  █   ░░  ▓▓  ▒▒  ░░  ▓▓  ▒▒  █   ░░  ▒▒

图例：
  ░ = 0任务 (灰色)
  ▒ = 1-3任务 (绿色)
  ▓ = 4-6任务 (黄色)
  █ = 7+任务 (红色)

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
📈 总计完成: 156 个任务
🔥 最活跃日期: 2025-10-15 (9个任务)
⚡ 当前连续: 7 天
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

---

## 🚀 使用示例

### 项目管理示例

```cpp
#include "ProjectManager.h"

int main() {
    ProjectManager pm;
    
    // 创建项目
    Project project("CSC3002课程项目", "游戏化任务管理系统", "#4CAF50");
    project.setTargetDate("2025-12-15");
    project.setTotalTasks(10);
    project.setCompletedTasks(3);
    
    int id = pm.createProject(project);
    
    // 查询项目
    Project* p = pm.getProject(id);
    cout << "项目名称: " << p->getName() << endl;
    cout << "项目进度: " << (p->getProgress() * 100) << "%" << endl;
    
    // 更新进度
    p->setCompletedTasks(5);
    pm.updateProjectProgress(id);
    
    return 0;
}
```

### 热力图示例

```cpp
#include "HeatmapVisualizer.h"

int main() {
    HeatmapVisualizer viz;
    
    // 添加任务数据
    viz.addTaskData("2025-10-01", 3);
    viz.addTaskData("2025-10-02", 5);
    viz.addTaskData("2025-10-03", 0);
    
    // 生成热力图
    cout << viz.generateHeatmap(90);
    
    // 生成月视图
    cout << viz.generateMonthView("2025-10");
    
    return 0;
}
```

---

## 🔨 编译运行

### 编译项目管理测试
```bash
g++ test_project.cpp src/Project.cpp src/ProjectManager.cpp -o test_project -I./include
.\test_project.exe
```

### 编译热力图测试
```bash
g++ test_heatmap.cpp src/HeatmapVisualizer.cpp -o test_heatmap -I./include
.\test_heatmap.exe
```

---

## 🔗 模块依赖与接口

### 依赖的模块

#### 1. 数据库模块 (成员D - 须安)

**需要的接口：**
```cpp
// ProjectDAO - 项目数据访问对象
class ProjectDAO {
public:
    int insert(const Project& project);           // 插入项目，返回ID
    Project* selectById(int id);                  // 查询单个项目
    vector<Project*> selectAll();                 // 查询所有项目
    bool update(const Project& project);          // 更新项目
    bool deleteById(int id);                      // 删除项目
};

// TaskDAO - 任务数据访问对象（用于查询项目下的任务）
class TaskDAO {
public:
    vector<Task*> selectByProjectId(int project_id);  // 查询项目的所有任务
    int countByProjectId(int project_id);             // 统计项目任务数
    int countCompletedByProjectId(int project_id);    // 统计完成的任务数
    
    // 热力图需要的接口
    map<string, int> getTaskCountByDate(int days);    // 获取每天的任务完成数
};
```

**数据库表结构需求：**
```sql
-- projects表
CREATE TABLE projects (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT,
    color_label TEXT,
    target_date TEXT,
    created_date TEXT DEFAULT CURRENT_TIMESTAMP
);

-- tasks表（需要有project_id关联）
CREATE TABLE tasks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    project_id INTEGER,
    completed BOOLEAN DEFAULT 0,
    completed_date TEXT,
    FOREIGN KEY (project_id) REFERENCES projects(id)
);
```

---

#### 2. 任务管理模块 (成员C - Kuang Wenqing)

**对接内容：**
- Task类中的 `project_id` 字段关联Project
- 创建任务时需要选择所属项目
- 提供接口供我查询项目下的任务

**需要的协作：**
```cpp
// 我需要调用任务模块的方法
TaskManager taskMgr;
vector<Task*> tasks = taskMgr.getTasksByProjectId(project_id);
```

---

### 提供给其他模块的接口

#### 给UI模块 (成员E - Mao Jingqi)

```cpp
// UI可以调用这些方法显示项目信息
ProjectManager pm;

// 获取所有项目
vector<Project*> projects = pm.getAllProjects();

// 获取项目进度
double progress = pm.calculateProgress(project_id);

// 获取热力图
HeatmapVisualizer viz;
string heatmap = viz.generateHeatmap(90);
```

---

## 📊 数据流图

```
用户创建Project
    ↓
ProjectManager.createProject()
    ↓
ProjectDAO.insert() → 数据库
    ↓
返回project_id
    ↓
用户创建Task时选择project_id
    ↓
TaskManager关联Task到Project
    ↓
ProjectManager.calculateProgress()
    ↓
查询TaskDAO获取任务完成情况
    ↓
更新Project进度
    ↓
HeatmapVisualizer显示完成热力图
```

---

## ⚠️ 当前状态

### ✅ 已完成
- Project类实现
- ProjectManager业务逻辑
- HeatmapVisualizer可视化
- 使用内存存储的完整功能演示

### ⏰ 待集成
- 等待DatabaseManager和DAO接口完成
- 将 `map<int, Project*>` 替换为 `ProjectDAO` 调用
- 集成真实的任务数据到热力图

### 🔄 集成步骤（后续）
1. 引入ProjectDAO和TaskDAO
2. 替换ProjectManager中的数据存储
3. 修改HeatmapVisualizer从数据库读取任务完成数据
4. 测试完整功能

---

## 📝 代码统计

| 文件 | 行数 |
|------|------|
| Project.h | 50 |
| Project.cpp | 100 |
| ProjectManager.h | 30 |
| ProjectManager.cpp | 120 |
| HeatmapVisualizer.h | 40 |
| HeatmapVisualizer.cpp | 160 |
| **总计** | **~500行** |

---

## 🎯 技术亮点

1. **面向对象设计** - 清晰的类职责划分
2. **ASCII艺术** - 创新的热力图可视化
3. **易于扩展** - 数据层和业务层分离
4. **可测试性** - 独立的测试程序
5. **代码规范** - 注释完整，命名清晰

---

## 📧 联系方式

**负责人**: 周天健  
**问题反馈**: 通过GitHub Issues或项目组讨论

---

*最后更新: 2025-11-07*
