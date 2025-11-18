# 🎯 智能任务管理系统（游戏化版本）

## 📝 项目简介

这是一个基于C++开发的智能任务管理系统，融合了游戏化元素，旨在通过经验值、等级、成就等机制提高用户的任务管理效率和积极性。系统采用SQLite3数据库进行数据持久化，提供完整的任务管理、项目管理、统计分析等功能。

### ✨ 核心特性

- 📋 **任务管理**：支持任务的创建、修改、删除、查询，任务优先级设置
- 📊 **项目管理**：任务分组管理，项目进度自动计算
- 🎮 **游戏化系统**：经验值、等级体系（20级）、成就系统
- 📈 **统计分析**：任务完成统计、生产力分析、热力图可视化
- 🔔 **提醒系统**：任务截止日期提醒、重复提醒支持
- 🍅 **番茄钟**：专注时间管理，提高工作效率
- 💎 **挑战系统**：日常、周常、月常挑战任务
- 🎨 **精美UI**：ANSI彩色控制台界面，交互友好

---

## 👥 团队成员及贡献度

### 项目组成员

| 成员 | 负责模块 | 代码量 | 完成度 | 主要贡献 |
|------|---------|--------|--------|---------|
| **成员A (Fei Yifan)** | 提醒系统、成就管理 | ~750行 | 70% | 提醒系统核心功能、成就管理器实现 |
| **成员B (Zhou Tianjian)** | 项目管理、热力图 | ~480行 | 90% | Project类、ProjectManager、基础热力图设计 |
| **成员C (Kuang Wenqing)** | 任务管理 | ~230行 | 60% | Task实体、TaskManager核心功能 |
| **成员D (Yu Zhixuan)** | 数据库、DAO层 | ~2500行 | 75% | DatabaseManager、各类DAO接口与实现 |
| **成员E (Mao Jingqi)** | 统计分析、UI、游戏化 | ~2700行 | 100% | 统计引擎、XP系统、UI管理器、主程序、热力图完整实现 |

### 详细贡献分析

#### 成员A (Fei Yifan) - 提醒与成就系统
**完成度**: 70% ⭐⭐⭐⭐

**已完成**:
- ✅ ReminderSystem类实现 (~280行)
- ✅ AchievementManager类实现 (~345行)
- ✅ 提醒类型支持（一次性、每日、每周、每月）
- ✅ 成就基础框架

**待完善**:
- ⏰ 提醒触发逻辑优化
- ⏰ 成就自动解锁条件判定
- ⏰ 成就进度实时跟踪

**代码文件**:
- `src/reminder/ReminderSystem.cpp` (281行)
- `src/achievement/AchievementManager.cpp` (345行)
- `include/reminder/ReminderSystem.h` (44行)

---

#### 成员B (Zhou Tianjian) - 项目管理模块
**完成度**: 90% ⭐⭐⭐⭐⭐

**已完成**:
- ✅ Project实体类完整实现 (~100行)
- ✅ ProjectManager业务逻辑 (~130行)
- ✅ 项目CRUD操作
- ✅ 项目进度自动计算
- ✅ 项目归档功能
- ✅ 热力图设计文档

**技术亮点**:
- 清晰的面向对象设计
- 完整的项目生命周期管理
- 与任务模块的良好集成

**代码文件**:
- `src/project/Project.cpp` (44行)
- `src/project/ProjectManager.cpp` (92行)
- `include/project/Project.h` (55行)
- `include/project/ProjectManager.h` (35行)

---

#### 成员C (Kuang Wenqing) - 任务管理模块
**完成度**: 60% ⭐⭐⭐

**已完成**:
- ✅ Task实体定义 (common/entities.h)
- ✅ TaskManager基础框架 (~150行)
- ✅ 任务基本数据结构

**待完善**:
- ⏰ 任务CRUD完整实现
- ⏰ 任务状态管理优化
- ⏰ 与项目模块深度集成
- ⏰ 任务筛选和排序功能

**代码文件**:
- `src/task/TaskManager.cpp` (132行)
- `src/task/task.cpp` (20行)
- `include/task/TaskManager.h` (55行)
- `include/task/task.h` (22行)

---

#### 成员D (Yu Zhixuan) - 数据库与DAO层
**完成度**: 75% ⭐⭐⭐⭐

**已完成**:
- ✅ DatabaseManager核心功能 (~622行)
- ✅ 9个DAO接口定义
- ✅ 6个DAO实现类
- ✅ SQLite3集成
- ✅ 数据库连接池管理

**已实现的DAO**:
- ProjectDAO (347行)
- TaskDAOImpl (428行)
- ReminderDAO (519行)
- AchievementDAO (395行)
- HeatmapVisualizerDao (297行)

**待完善**:
- ⏰ 挑战系统实现 (0%)
- ⏰ 部分DAO方法优化

**技术亮点**:
- 单例模式设计
- 完整的错误处理
- SQL注入防护

**代码文件**:
- `src/database/databasemanager.cpp` (622行)
- `src/database/DAO/ProjectDAO.cpp` (347行)
- `src/database/DAO/TaskDAOImpl.cpp` (428行)
- `src/database/DAO/ReminderDAO.cpp` (519行)
- `src/database/DAO/AchievementDAO.cpp` (395行)
- `include/database/DatabaseManager.h` (127行)

---

#### 成员E (Mao Jingqi) - 统计分析、游戏化、UI
**完成度**: 100% ⭐⭐⭐⭐⭐

**杰出贡献**: 超额完成任务，代码量占项目总量的40%以上

**已完成**:
1. **统计分析引擎** (~640行)
   - ✅ 多维度任务统计
   - ✅ 生产力分析
   - ✅ 时间分析
   - ✅ 连续打卡记录
   - ✅ 自动报告生成（日报、周报、月报）

2. **经验值与等级系统** (~470行)
   - ✅ 20级等级体系
   - ✅ 科学的经验值曲线
   - ✅ 多种XP来源（任务完成、番茄钟、连续打卡）
   - ✅ 升级动画和进度条

3. **热力图可视化** (~363行)
   - ✅ 90天任务完成热力图
   - ✅ 月视图/周视图切换
   - ✅ ASCII艺术渲染
   - ✅ 统计信息显示

4. **UI管理器** (~832行)
   - ✅ ANSI彩色控制台界面
   - ✅ 多级菜单系统
   - ✅ 用户输入验证
   - ✅ 友好的交互设计

5. **主程序** (~213行)
   - ✅ 系统初始化
   - ✅ 主循环控制
   - ✅ 资源管理

6. **构建系统**
   - ✅ Makefile配置 (87行)
   - ✅ 自动化编译脚本

7. **项目文档**
   - ✅ BUILD_GUIDE.md (249行)
   - ✅ 开发指南和技术文档 (1000+行)

**技术亮点**:
- 直接使用SQLite C API进行高效查询
- 完善的错误处理和边界检查
- 优秀的代码结构和注释
- 专业的UI设计

**代码文件**:
- `src/statistics/StatisticsAnalyzer.cpp` (470行)
- `src/gamification/XPSystem.cpp` (314行)
- `src/HeatmapVisualizer/HeatmapVisualizer.cpp` (315行)
- `src/ui/UIManager.cpp` (607行)
- `src/main.cpp` (207行)
- `include/statistics/StatisticsAnalyzer.h` (172行)
- `include/gamification/XPSystem.h` (148行)
- `include/HeatmapVisualizer/HeatmapVisualizer.h` (40行)
- `include/ui/UIManager.h` (194行)

---

## 🏗️ 系统架构

### 分层架构

```
┌─────────────────────────────────────────┐
│          表示层 (Presentation)          │
│         UIManager (彩色控制台UI)         │
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│           业务逻辑层 (Business)          │
│  TaskManager │ ProjectManager │ XPSystem│
│  ReminderSystem │ AchievementManager   │
│  StatisticsAnalyzer │ HeatmapVisualizer│
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│          数据访问层 (Data Access)        │
│  TaskDAO │ ProjectDAO │ ReminderDAO    │
│  AchievementDAO │ ExperienceDAO        │
└─────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────┐
│        数据库层 (Database - SQLite3)     │
│  tasks │ projects │ reminders │ users  │
└─────────────────────────────────────────┘
```

### 核心模块

1. **数据库管理 (DatabaseManager)**
   - 单例模式
   - 连接池管理
   - 事务支持
   - SQL执行引擎

2. **任务管理 (TaskManager)**
   - 任务CRUD操作
   - 任务状态管理
   - 优先级管理
   - 标签系统

3. **项目管理 (ProjectManager)**
   - 项目CRUD操作
   - 进度自动计算
   - 任务分组
   - 项目归档

4. **游戏化系统 (XPSystem)**
   - 经验值计算
   - 等级管理
   - 升级动画
   - 排行榜

5. **统计分析 (StatisticsAnalyzer)**
   - 任务统计
   - 时间分析
   - 生产力评估
   - 报告生成

6. **提醒系统 (ReminderSystem)**
   - 定时提醒
   - 重复提醒
   - 提醒管理

7. **成就系统 (AchievementManager)**
   - 成就定义
   - 解锁条件
   - 进度跟踪

---

## 🚀 快速开始

### 环境要求

- **操作系统**: Linux / macOS / Windows (WSL)
- **编译器**: g++ 7.0+ (支持C++17)
- **数据库**: SQLite3 3.0+
- **构建工具**: Make (可选)

### 安装依赖

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install g++ make libsqlite3-dev
```

#### macOS
```bash
brew install gcc make sqlite3
```

#### Windows (WSL)
```bash
sudo apt-get install g++ make libsqlite3-dev
```

### 编译项目

#### 使用Makefile（推荐）
```bash
# 克隆项目
git clone https://github.com/Y-xvan/Smart-Task-Management-System-with-Gamification.git
cd Smart-Task-Management-System-with-Gamification

# 编译
make

# 运行
make run
```

#### 手动编译
```bash
# 创建构建目录
mkdir -p build bin

# 编译所有源文件
g++ -std=c++17 -Wall -Wextra -I./include -I./common \
    src/main.cpp \
    src/database/databasemanager.cpp \
    src/database/DAO/ProjectDAO.cpp \
    src/project/Project.cpp \
    src/project/ProjectManager.cpp \
    src/statistics/StatisticsAnalyzer.cpp \
    src/gamification/XPSystem.cpp \
    src/HeatmapVisualizer/HeatmapVisualizer.cpp \
    src/ui/UIManager.cpp \
    -o bin/task_manager -lsqlite3

# 运行
./bin/task_manager
```

### 清理构建

```bash
make clean
```

---

## 📖 使用指南

### 启动程序

```bash
./bin/task_manager
```

### 主菜单功能

程序启动后，会显示以下主菜单：

```
╔═══════════════════════════════════════════════════╗
║         🎯 智能任务管理系统 (游戏化版本)           ║
╚═══════════════════════════════════════════════════╝

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
👤 用户: Player  🏆 等级: 5  ⭐ XP: 450/800
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

1. 📋 任务管理
2. 📊 项目管理
3. 📈 统计分析
4. 📅 热力图可视化
5. 🏆 经验值系统
6. 🔔 提醒管理
7. ⚙️  设置
0. 🚪 退出

请选择功能 (0-7):
```

### 核心功能使用

#### 1. 任务管理
- **创建任务**: 输入任务标题、描述、优先级、截止日期
- **查看任务**: 查看所有任务或按状态筛选
- **完成任务**: 标记任务完成并获得经验值奖励
- **删除任务**: 删除不需要的任务

#### 2. 项目管理
- **创建项目**: 设置项目名称、描述、颜色标签
- **查看项目**: 列出所有项目及其进度
- **项目详情**: 查看项目下的所有任务
- **归档项目**: 归档已完成的项目

#### 3. 统计分析
- **任务统计**: 总任务数、完成数、完成率
- **时间分析**: 今日、本周、本月完成情况
- **生产力报告**: 平均完成时间、效率趋势
- **连续打卡**: 当前连续完成任务天数

#### 4. 热力图可视化
- **90天热力图**: 以颜色深度显示每日任务完成情况
- **月视图**: 查看特定月份的任务分布
- **周视图**: 查看特定周的详细数据
- **统计信息**: 最活跃日期、总完成任务数

#### 5. 经验值系统
- **查看等级**: 当前等级、经验值、升级进度
- **经验值历史**: 查看获得经验值的历史记录
- **排行榜**: 与其他用户比较经验值

---

## 📊 数据库结构

### 核心数据表

#### tasks 表
```sql
CREATE TABLE tasks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    description TEXT,
    priority INTEGER DEFAULT 1,
    due_date TEXT,
    completed BOOLEAN DEFAULT 0,
    tags TEXT,
    project_id INTEGER,
    pomodoro_count INTEGER DEFAULT 0,
    completed_date TEXT,
    created_date TEXT DEFAULT CURRENT_TIMESTAMP,
    updated_date TEXT DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (project_id) REFERENCES projects(id)
);
```

#### projects 表
```sql
CREATE TABLE projects (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    description TEXT,
    color_label TEXT,
    progress REAL DEFAULT 0.0,
    total_tasks INTEGER DEFAULT 0,
    completed_tasks INTEGER DEFAULT 0,
    target_date TEXT,
    archived BOOLEAN DEFAULT 0,
    created_date TEXT DEFAULT CURRENT_TIMESTAMP,
    updated_date TEXT DEFAULT CURRENT_TIMESTAMP
);
```

#### users 表
```sql
CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    current_level INTEGER DEFAULT 1,
    total_xp INTEGER DEFAULT 0,
    created_date TEXT DEFAULT CURRENT_TIMESTAMP
);
```

#### experience_records 表
```sql
CREATE TABLE experience_records (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER,
    amount INTEGER NOT NULL,
    source TEXT,
    description TEXT,
    timestamp TEXT DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id)
);
```

#### reminders 表
```sql
CREATE TABLE reminders (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    task_id INTEGER,
    reminder_time TEXT NOT NULL,
    type TEXT,
    status TEXT DEFAULT 'PENDING',
    message TEXT,
    created_date TEXT DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (task_id) REFERENCES tasks(id)
);
```

#### achievements 表
```sql
CREATE TABLE achievements (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    description TEXT,
    icon TEXT,
    criteria TEXT,
    xp_reward INTEGER DEFAULT 0,
    created_date TEXT DEFAULT CURRENT_TIMESTAMP
);
```

---

## 🎮 游戏化机制

### 经验值来源

| 行为 | 获得XP | 说明 |
|------|--------|------|
| 完成低优先级任务 | 10 XP | 完成一个低优先级任务 |
| 完成中优先级任务 | 20 XP | 完成一个中优先级任务 |
| 完成高优先级任务 | 30 XP | 完成一个高优先级任务 |
| 完成番茄钟 | 5 XP | 完成一个25分钟番茄钟 |
| 连续打卡3天 | 50 XP | 连续3天完成任务 |
| 连续打卡7天 | 100 XP | 连续7天完成任务 |
| 完成挑战 | 50-200 XP | 根据挑战难度 |
| 解锁成就 | 100-500 XP | 根据成就稀有度 |

### 等级体系

系统采用20级等级体系，升级所需经验值呈指数增长：

```
等级 1:  0 XP      → 等级 6:  1600 XP
等级 2:  100 XP    → 等级 7:  2400 XP
等级 3:  300 XP    → 等级 8:  3400 XP
等级 4:  600 XP    → 等级 9:  4600 XP
等级 5:  1000 XP   → 等级 10: 6000 XP
...                → 等级 20: 50000 XP
```

### 成就系统

成就分为以下几类：

1. **任务成就**
   - 🎯 初出茅庐：完成第一个任务
   - 🎯 劳模：完成100个任务
   - 🎯 超级劳模：完成500个任务

2. **连续打卡成就**
   - 🔥 三天火热：连续3天打卡
   - 🔥 一周坚持：连续7天打卡
   - 🔥 月度坚持：连续30天打卡

3. **专注成就**
   - 🍅 番茄新手：完成10个番茄钟
   - 🍅 番茄达人：完成100个番茄钟
   - 🍅 专注大师：完成500个番茄钟

4. **项目成就**
   - 📊 项目启动：创建第一个项目
   - 📊 项目经理：完成10个项目
   - 📊 项目大师：完成50个项目

---

## 📈 项目进度

### 整体完成度：75%

### 模块完成情况

| 模块 | 完成度 | 状态 |
|------|--------|------|
| 数据库管理 | 85% | ✅ 核心完成 |
| 任务管理 | 60% | ⚠️ 待完善 |
| 项目管理 | 90% | ✅ 功能完整 |
| 游戏化系统 | 100% | ✅ 完成 |
| 统计分析 | 100% | ✅ 完成 |
| UI管理器 | 100% | ✅ 完成 |
| 提醒系统 | 70% | ⚠️ 待优化 |
| 成就系统 | 70% | ⚠️ 待优化 |
| 番茄钟 | 30% | ⏰ 待实现 |
| 挑战系统 | 0% | ⏰ 待实现 |

### 下一步计划

#### 短期目标（1-2周）
- [ ] 完善TaskManager核心功能
- [ ] 优化提醒触发逻辑
- [ ] 实现成就自动解锁
- [ ] 完成番茄钟功能
- [ ] 添加单元测试

#### 中期目标（3-4周）
- [ ] 实现挑战系统
- [ ] 性能优化
- [ ] 代码重构
- [ ] 完善文档
- [ ] 用户手册

#### 长期目标
- [ ] 多用户支持
- [ ] 数据导入导出
- [ ] 跨平台GUI版本
- [ ] 云同步功能
- [ ] 移动端适配

---

## 🛠️ 技术栈

- **编程语言**: C++17
- **数据库**: SQLite3
- **构建工具**: Make / g++
- **设计模式**: 单例模式、DAO模式、MVC架构
- **代码规范**: Google C++ Style Guide

---

## 📁 项目结构

```
Smart-Task-Management-System-with-Gamification/
├── bin/                          # 编译后的可执行文件
│   └── task_manager
├── build/                        # 编译中间文件
├── common/                       # 公共定义
│   └── entities.h               # 实体类定义
├── include/                      # 头文件目录
│   ├── database/                # 数据库相关头文件
│   │   ├── DatabaseManager.h
│   │   └── DAO/                 # 数据访问对象接口
│   │       ├── TaskDAO.h
│   │       ├── ProjectDAO.h
│   │       ├── ReminderDAO.h
│   │       ├── AchievementDAO.h
│   │       ├── ExperienceDAO.h
│   │       └── StatisticsDAO.h
│   ├── task/                    # 任务管理
│   │   ├── TaskManager.h
│   │   └── task.h
│   ├── project/                 # 项目管理
│   │   ├── ProjectManager.h
│   │   └── Project.h
│   ├── gamification/            # 游戏化系统
│   │   └── XPSystem.h
│   ├── statistics/              # 统计分析
│   │   └── StatisticsAnalyzer.h
│   ├── HeatmapVisualizer/       # 热力图
│   │   └── HeatmapVisualizer.h
│   ├── ui/                      # 用户界面
│   │   └── UIManager.h
│   ├── reminder/                # 提醒系统
│   │   └── ReminderSystem.h
│   ├── achievement/             # 成就系统
│   │   └── AchievementManager.h
│   └── Pomodoro/                # 番茄钟
│       └── pomodoro.h
├── src/                         # 源文件目录
│   ├── main.cpp                 # 主程序入口
│   ├── database/                # 数据库实现
│   ├── task/                    # 任务管理实现
│   ├── project/                 # 项目管理实现
│   ├── gamification/            # 游戏化实现
│   ├── statistics/              # 统计分析实现
│   ├── HeatmapVisualizer/       # 热力图实现
│   ├── ui/                      # UI实现
│   ├── reminder/                # 提醒系统实现
│   ├── achievement/             # 成就系统实现
│   └── Pomodoro/                # 番茄钟实现
├── Makefile                     # 构建配置文件
├── README.md                    # 项目说明（英文）
├── README_CN.md                 # 项目说明（中文）
├── BUILD_GUIDE.md              # 编译指南
├── PROGRESS_SUMMARY_CN.md      # 进度摘要
└── PROJECT_PROGRESS_REPORT.md  # 详细进度报告
```

---

## 📄 代码统计

### 总体统计
- **总代码行数**: ~12,500行
- **头文件**: ~3,200行
- **源文件**: ~5,500行
- **文档**: ~3,800行

### 模块代码分布
```
数据库模块:    3,234行 (26%)
UI模块:         801行 (6%)
统计分析:       642行 (5%)
游戏化系统:     462行 (4%)
成就系统:       491行 (4%)
提醒系统:       421行 (3%)
热力图:         355行 (3%)
任务管理:       229行 (2%)
项目管理:       226行 (2%)
其他:           139行 (1%)
```

---

## 🤝 贡献指南

欢迎贡献代码！请遵循以下步骤：

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

### 代码规范
- 使用C++17标准
- 遵循Google C++编码规范
- 添加适当的注释
- 编写单元测试

---

## 🐛 已知问题

1. **任务管理模块**: 部分CRUD操作待完善
2. **提醒系统**: 重复提醒触发逻辑需优化
3. **成就系统**: 自动解锁条件判定待实现
4. **番茄钟**: 计时功能不完整
5. **挑战系统**: 尚未实现

详细问题请查看 [Issues](https://github.com/Y-xvan/Smart-Task-Management-System-with-Gamification/issues)

---

## 📞 联系方式

- **项目主页**: https://github.com/Y-xvan/Smart-Task-Management-System-with-Gamification
- **问题反馈**: 通过 GitHub Issues 提交

---

## 📜 开源协议

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件

---

## 🙏 致谢

感谢所有团队成员的辛勤付出和贡献！

特别感谢：
- **成员E (Mao Jingqi)**: 超额完成任务，为项目奠定坚实基础
- **成员D (Yu Zhixuan)**: 完善的数据库架构设计
- **成员B (Zhou Tianjian)**: 优秀的项目管理模块实现
- **成员A (Fei Yifan)**: 创新的提醒和成就系统
- **成员C (Kuang Wenqing)**: 任务管理核心功能开发

---

## 📚 相关文档

- [编译指南](BUILD_GUIDE.md)
- [进度摘要](PROGRESS_SUMMARY_CN.md)
- [详细进度报告](PROJECT_PROGRESS_REPORT.md)
- [任务管理器分析](TASKMANAGER_ANALYSIS.md)
- [Include路径说明](INCLUDE_PATH_EXPLANATION.md)

---

**最后更新**: 2025-11-18  
**版本**: 1.0.0  
**CSC3002 课程项目**

---

*让任务管理变得有趣，让进步看得见！* 🚀
