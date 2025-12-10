# Smart Task Management System with Gamification

A task management application with RPG-style gamification elements, featuring both **Console** and **GUI** interfaces with a "悬赏令" (Bounty Board) theme.

## ✨ Features

- **Dual Interface**: Choose between Console (terminal-based) or GUI (Qt/QML) interface
- **Gamification**: RPG-style XP system, levels, and achievements
- **Project Management**: Organize tasks into projects
- **Pomodoro Timer**: Focus mode with integrated timer
- **Statistics & Analytics**: Track your productivity
- **Reminder System**: Get notified about important tasks
- **Cross-platform**: Works on Windows, Linux, and macOS

## 🎮 Interface Options

### Console Version (Terminal UI)
- Full-featured terminal interface with colorful ASCII art
- Works anywhere without Qt dependency
- Perfect for server environments or minimalist setups
- Executable: `task_manager` / `task_manager.exe`

### GUI Version (Qt/QML)
- Modern, responsive interface with smooth animations
- "悬赏令" (Bounty Board) themed task cards
- RPG-style HUD showing level, XP, and streaks
- Interactive drag-and-drop functionality
- Executable: `task_manager_gui` / `task_manager_gui.exe`

## 🚀 Quick Start

### Using CMake (Recommended for GUI)

**Windows:**
```powershell
.\build_cmake.ps1
cd build\bin
.\task_manager_gui.exe
```

**Linux/macOS:**
```bash
./build_cmake.sh
cd build/bin
./task_manager_gui
```

### Using Makefile (Console Only)

```bash
make
./bin/task_manager
```

## 📖 Detailed Build Instructions

See [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md) for comprehensive build instructions including:
- Prerequisites and dependencies
- Platform-specific build steps
- Troubleshooting common issues
- Advanced build options

## 🛠️ Prerequisites

### Required for All Versions
- C++17 compatible compiler
- CMake 3.16+ (for CMake builds)
- SQLite3

### Additional Requirements for GUI
- Qt 5.15+ or Qt 6.x
  - Qt Core
  - Qt Gui
  - Qt Qml
  - Qt Quick

## 📂 Project Structure

```
├── src/
│   ├── main.cpp              # Console application entry point
│   ├── main_gui.cpp          # GUI application entry point
│   ├── database/             # Database layer
│   ├── task/                 # Task management
│   ├── project/              # Project management
│   ├── gamification/         # XP and level system
│   ├── statistics/           # Analytics
│   ├── reminder/             # Reminder system
│   ├── achievement/          # Achievement system
│   └── ui/                   # UI components (Qt models)
├── include/                  # Header files
├── resources/
│   ├── qml.qrc              # Qt resource file
│   └── qml/                 # QML UI components
│       ├── MainView.qml     # Main window
│       ├── QuestView.qml    # Task list view
│       ├── QuestCard.qml    # Bounty board styled cards
│       ├── ProjectView.qml  # Project management
│       ├── FocusView.qml    # Pomodoro timer
│       ├── StatsView.qml    # Statistics dashboard
│       └── ReminderView.qml # Reminders
├── sqlite/                  # SQLite library
├── CMakeLists.txt           # CMake build configuration
├── Makefile                 # Traditional Makefile (console)
├── build_cmake.sh           # Linux/macOS build script
└── build_cmake.ps1          # Windows build script
```

## 🎯 Usage

### Console Interface
Run the console version and follow the on-screen menu:
```bash
./task_manager
```

### GUI Interface
Double-click the GUI executable or run from terminal:
```bash
./task_manager_gui
```

The GUI features:
- **Quests Tab**: View and manage tasks with bounty board cards
- **Projects Tab**: Organize tasks into projects
- **Focus Tab**: Use Pomodoro timer for focused work
- **Stats Tab**: View your productivity statistics
- **Alerts Tab**: Manage reminders

## 🐛 Troubleshooting

### GUI executable runs but window doesn't appear (Windows)

**This issue has been fixed!** The CMakeLists.txt now properly configures:
- WIN32 flag for GUI applications
- Resource compilation for QML files
- Proper Windows subsystem settings

If you still experience issues:
1. Make sure you built with CMake (not the old Makefile)
2. Ensure Qt is properly installed
3. Check that `sqlite3.dll` is in the same directory as the executable
4. Try running from command line to see any error messages

### Qt not found during build

Add Qt to your PATH or specify the location:
```powershell
# Windows
.\build_cmake.ps1 -QtPath "C:\Qt\6.x\mingw_64\bin"
```

### Database errors

If you encounter database errors:
1. Make sure `task_manager.db` is writable
2. Try deleting the database file to recreate it
3. Check that SQLite3 is properly installed

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## 📝 License

[Add your license information here]

## 🙏 Acknowledgments

- Built with Qt for the GUI interface
- SQLite for database management
- Inspired by RPG game mechanics for productivity
