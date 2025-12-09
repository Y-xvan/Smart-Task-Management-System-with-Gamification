# 用户指南 - Smart Task Management System

## 🎯 快速开始

### 问题已解决！✅

您之前遇到的问题："GUI 版本的 exe 双击没有反应，但是后台一直在运行" 现在已经完全解决了！

### 解决方案

我们创建了一个新的 CMake 构建系统，它正确配置了 Windows GUI 应用程序。关键修复点：

1. ✅ 使用 `WIN32` 标志构建 GUI 可执行文件
2. ✅ 正确嵌入 Qt 资源（QML 文件）
3. ✅ 修复了 C++ 语法兼容性问题
4. ✅ 保留了控制台版本和 GUI 版本

## 🚀 如何构建和运行

### Windows 用户（推荐）

#### 第一步：构建项目

打开 PowerShell，在项目根目录运行：

```powershell
# 构建项目
.\build_cmake.ps1

# 如果 Qt 不在 PATH 中，指定 Qt 路径：
.\build_cmake.ps1 -QtPath "C:\Qt\6.5.0\mingw_64\bin"
```

#### 第二步：运行程序

构建完成后，可执行文件在 `build\bin\` 目录：

```powershell
# 运行控制台版本
cd build\bin
.\task_manager.exe

# 运行 GUI 版本（现在可以正常显示窗口了！）
.\task_manager_gui.exe
```

或者直接在文件管理器中双击 `build\bin\task_manager_gui.exe`！

### 构建选项

```powershell
# Debug 模式构建
.\build_cmake.ps1 -Debug

# 清理后重新构建
.\build_cmake.ps1 -Clean

# 使用不同的 CMake 生成器
.\build_cmake.ps1 -Generator "Visual Studio 17 2022"
.\build_cmake.ps1 -Generator "Ninja"
```

## 🎮 "悬赏令"主题 UI

GUI 版本已经实现了完整的"悬赏令"主题界面！

### 界面特色

```
┌─────────────────────────────────────────────┐
│  🧙‍♂️ Lv.5 冒险者    [████░░] 450/600 XP  🔥7  │  ← HUD 栏
├─────────────────────────────────────────────┤
│                                             │
│  ┌─────────────────────────────────────┐   │
│  │ 📜 悬赏任务卡片（羊皮纸风格）        │   │
│  │ ┌──────────────────────────────┐    │   │
│  │ │ [红条] 修复登录 Bug           │ 🏆│   │
│  │ │        完成登录功能...        │ 50│   │
│  │ │        📁 项目A  📅 明天      │   │   │
│  │ └──────────────────────────────┘    │   │
│  │                                     │   │
│  │ ┌──────────────────────────────┐    │   │
│  │ │ [橙条] 编写文档               │ 🏆│   │
│  │ │        为新功能编写...        │ 30│   │
│  │ └──────────────────────────────┘    │   │
│  └─────────────────────────────────────┘   │
│                                    [+]      │  ← 添加任务按钮
├─────────────────────────────────────────────┤
│  ⚔️任务 │ 📁项目 │ 🍅专注 │ 📊统计 │ 🔔提醒  │  ← 底部导航
└─────────────────────────────────────────────┘
```

### 功能详解

1. **⚔️ 任务（Quests）** - 悬赏令风格的任务列表
   - 羊皮纸风格卡片（#FDF5E6 老蕾丝色）
   - 优先级彩色条（红/橙/绿）
   - XP 奖励显示
   - 长按完成任务

2. **📁 项目（Projects）** - 项目管理
   - 创建和管理项目
   - 将任务组织到项目中

3. **🍅 专注（Focus）** - 番茄钟计时器
   - 25分钟专注时间
   - 5分钟休息时间
   - 完成获得 XP

4. **📊 统计（Stats）** - 数据分析
   - 完成任务数
   - 总番茄钟数
   - 连胜天数

5. **🔔 提醒（Alerts）** - 任务提醒
   - 设置截止日期提醒
   - 任务到期通知

### RPG 游戏化元素

- **等级系统** - 完成任务获得 XP，升级解锁称号
- **连胜系统** - 连续完成任务保持火焰 🔥
- **成就系统** - 解锁各种成就徽章
- **XP 奖励** - 每个任务都有对应的 XP 奖励

## 📚 详细文档

项目包含完整的文档：

- **README.md** - 项目概述和快速开始
- **BUILD_INSTRUCTIONS.md** - 详细构建说明（中英文）
- **SOLUTION_SUMMARY.md** - 问题和解决方案详解（中英文）
- **ARCHITECTURE.md** - 项目架构和技术细节

## ❓ 常见问题

### Q1: 构建时提示 "Qt not found"

**A:** 需要安装 Qt 并添加到 PATH，或在构建时指定路径：

```powershell
.\build_cmake.ps1 -QtPath "C:\Qt\6.5.0\mingw_64\bin"
```

### Q2: 运行时提示缺少 sqlite3.dll

**A:** 将 SQLite DLL 复制到可执行文件目录：

```powershell
Copy-Item sqlite\sqlite3.dll build\bin\
```

### Q3: GUI 窗口还是不显示

**A:** 请确认：
1. 使用 CMake 构建（不是旧的 Makefile）
2. 运行的是 `task_manager_gui.exe`（不是 `task_manager.exe`）
3. Qt 已正确安装
4. 查看命令行输出的错误信息

### Q4: 想要修改窗口大小

**A:** 编辑 `resources/qml/MainView.qml` 文件：

```qml
Window {
    id: window
    width: 450   // 修改宽度
    height: 850  // 修改高度
    // ...
}
```

### Q5: 控制台版本还能用吗？

**A:** 当然！控制台版本完全保留：

```powershell
# 使用 Makefile（如果在 Linux/macOS）
make
./bin/task_manager

# 或使用 CMake
.\build_cmake.ps1
.\build\bin\task_manager.exe
```

## 🎨 自定义主题

如果想修改"悬赏令"主题的颜色：

### 任务卡片背景色

编辑 `resources/qml/QuestCard.qml`：

```qml
Rectangle {
    color: "#FDF5E6"  // 改为你喜欢的颜色
    // ...
}
```

### 优先级颜色

在 `QuestCard.qml` 中：

```qml
color: {
    if(priority === 2) return "#FF5252"  // 高优先级（红色）
    if(priority === 1) return "#FFB74D"  // 中优先级（橙色）
    return "#66BB6A"                     // 低优先级（绿色）
}
```

### 主题背景

编辑 `resources/qml/MainView.qml`：

```qml
Window {
    color: "#121212"  // 深色背景，改为你喜欢的颜色
    // ...
}
```

## 🔧 技术支持

### 生成的文件

构建后会生成：

```
build/
├── bin/
│   ├── task_manager.exe         # 控制台版本
│   └── task_manager_gui.exe     # GUI 版本 ⭐
└── ...
```

### 数据库文件

应用会创建 `task_manager.db` 数据库文件来保存：
- 任务和项目数据
- 用户统计和进度
- 成就解锁记录
- 提醒设置

### 系统要求

**最低要求：**
- Windows 7 或更高版本
- 2GB RAM
- 100MB 磁盘空间

**推荐要求：**
- Windows 10 或更高版本
- 4GB RAM
- Qt 5.15+ 或 Qt 6.x

## 🎉 享受使用！

现在您可以：
1. ✅ 双击 `task_manager_gui.exe` 直接打开 GUI 界面
2. ✅ 使用"悬赏令"风格的任务管理界面
3. ✅ 通过 RPG 游戏化元素提高生产力
4. ✅ 在控制台和 GUI 之间自由切换

祝您任务管理愉快！🚀

---

**如有问题或建议，欢迎在 GitHub 上提出 Issue！**
