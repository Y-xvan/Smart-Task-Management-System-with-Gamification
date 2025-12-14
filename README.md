# 🚀 Smart Task Management System with Gamification

<div align="center">

![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![C++](https://img.shields.io/badge/C++-17-00599C?logo=cplusplus&logoColor=white)
![SQLite](https://img.shields.io/badge/SQLite-3-003B57?logo=sqlite&logoColor=white)
![License](https://img.shields.io/badge/license-MIT-green.svg)

**A modern, gamified task management system that makes productivity fun!**

*Complete tasks, earn XP, unlock achievements, and track your progress with beautiful visualizations.*

</div>

---

## ✨ Features

### 📋 Task Management
- **Create, edit, and delete tasks** with rich metadata
- **Priority levels** (Low ★, Medium ★★, High ★★★)
- **Due date tracking** with visual urgency indicators
- **Tags and descriptions** for better organization
- **Pomodoro estimation** for time management

### 📁 Project Organization
- **Group tasks into projects** with custom colors
- **Progress tracking** with visual progress bars
- **Target dates** for project milestones
- **Archive completed projects**

### ⏰ Reminders
- **Flexible scheduling** with precise date/time settings
- **Recurrence options**: Once, Daily, Weekly, Monthly
- **Browser notifications** for timely alerts
- **Task linking** to associate reminders with specific tasks
- **Snooze functionality** for quick reschedules

### 🍅 Pomodoro Timer
- **Classic Pomodoro technique** implementation
- **25-minute work sessions** with breaks
- **Short breaks** (5 min) and **Long breaks** (15 min)
- **Pause and resume** functionality
- **Session tracking** and statistics
- **XP rewards** for completed sessions

### 🎮 Gamification System
- **Experience Points (XP)** system
  - +10-50 XP per task (based on priority)
  - +100 XP for completing projects
  - +5 XP per Pomodoro session
  - Streak bonuses for consistent activity
- **Level progression** with unique titles
- **Achievements** with progress tracking
- **Visual celebrations** for milestones

### 📊 Statistics & Analytics
- **Activity heatmap** (GitHub-style, 90-day view)
- **Daily, weekly, and monthly reports**
- **Completion rate tracking**
- **Streak monitoring**
- **Comprehensive statistics dashboard**

### 🌐 Web Interface
- **Modern, responsive SPA** design
- **Dark theme** with glassmorphism effects
- **Smooth animations** and micro-interactions
- **No external dependencies** for the backend
- **Real-time updates** and live data

### 📱 Native Qt/QML GUI (Optional)
- **Mobile-style RPG interface** with gamified design
- **Dark theme** with HUD-style status bar
- **XP bar and level display** with real-time updates
- **Streak tracking** with flame indicator
- **Tab-based navigation**: Quests, Projects, Focus, Stats, Alerts
- **Achievement badges** with unlock animations

---

## 🖥️ Screenshots

The web interface features:
- 🎨 Beautiful gradient backgrounds
- 💎 Glassmorphism card effects
- ✨ Smooth hover animations
- 🌈 Colorful progress indicators
- 🔔 Elegant notification popups

---

## 🛠️ Technology Stack

| Component | Technology |
|-----------|------------|
| **Backend** | C++17 |
| **Database** | SQLite3 |
| **Web Server** | Built-in HTTP server (std::thread + sockets) |
| **Frontend** | HTML5, CSS3, Vanilla JavaScript |
| **Native GUI** | Qt 6, QML (optional) |
| **Build System** | Make |

---

## 📦 Prerequisites

- **C++17** compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- **SQLite3** library
- **Make** build system
- **pthread** library (Linux/macOS)
- **Qt 6** with QML (optional, for native GUI)

### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install build-essential libsqlite3-dev

# Optional: For Qt/QML GUI
sudo apt-get install qt6-base-dev qt6-declarative-dev
```

### macOS
```bash
brew install sqlite3

# Optional: For Qt/QML GUI
brew install qt
```

### Windows
- Install MinGW-w64 or Visual Studio
- SQLite3 DLL is included in the `sqlite/` directory

---

## 🚀 Quick Start

### 1. Clone the repository
```bash
git clone https://github.com/Y-xvan/Smart-Task-Management-System-with-Gamification.git
cd Smart-Task-Management-System-with-Gamification
```

### 2. Build the project
```bash
make
```

### 3. Run the application
```bash
make run
```

Or directly:
```bash
./bin/task_manager
```

The web interface will automatically open in your default browser at `http://127.0.0.1:8787`

### Alternative: Console Mode
```bash
./bin/task_manager --console
```

---

## 📁 Project Structure

```
Smart-Task-Management-System-with-Gamification/
├── src/                          # Source code
│   ├── main.cpp                  # Main entry point (Web UI)
│   ├── main_gui.cpp              # Qt/QML GUI entry point
│   ├── task/                     # Task management
│   ├── project/                  # Project management
│   ├── reminder/                 # Reminder system
│   ├── Pomodoro/                 # Pomodoro timer
│   ├── achievement/              # Achievement system
│   ├── gamification/             # XP and leveling
│   ├── statistics/               # Statistics analyzer
│   ├── HeatmapVisualizer/        # Heatmap generator
│   ├── database/                 # Database layer
│   │   └── DAO/                  # Data Access Objects
│   ├── ui/                       # Console UI
│   └── web/                      # Web server
├── include/                      # Header files
├── resources/
│   ├── web/                      # Web frontend
│   │   ├── index.html            # SPA entry point
│   │   └── static/
│   │       ├── style.css         # Styles
│   │       └── main.js           # JavaScript
│   ├── qml/                      # Qt/QML UI components
│   │   ├── MainView.qml          # Main window with HUD
│   │   ├── QuestView.qml         # Tasks list view
│   │   ├── QuestCard.qml         # Task card component
│   │   ├── ProjectView.qml       # Projects view
│   │   ├── ProjectCard.qml       # Project card component
│   │   ├── FocusView.qml         # Pomodoro timer view
│   │   ├── StatsView.qml         # Statistics & achievements
│   │   ├── ReminderView.qml      # Reminders view
│   │   ├── NewTaskDialog.qml     # New task dialog
│   │   └── AppLauncher.qml       # App launcher
│   └── qml.src                   # QML resource file
├── sqlite/                       # SQLite library (Windows)
├── common/                       # Common utilities
├── Makefile                      # Build configuration
├── build.ps1                     # Windows build script
└── README.md                     # This file
```

---

## 🎯 Usage Guide

### Creating a Task
1. Navigate to **Tasks & Projects** section
2. Fill in the task details:
   - **Name**: Task title (required)
   - **Description**: Additional details
   - **Priority**: Low/Medium/High
   - **Due Date**: Deadline (YYYY-MM-DD)
   - **Project**: Optional project assignment
3. Click **Create Task**

### Starting a Pomodoro Session
1. Go to the **Pomodoro** section
2. Click **🍅 Start Work** for a 25-minute session
3. Focus on your task until the timer ends
4. Take a **☕ Short Break** (5 min) or **🛋️ Long Break** (15 min)
5. Repeat! Complete 4 work sessions for optimal productivity

### Setting Reminders
1. Navigate to **Reminders** section
2. Enter:
   - **Title**: Reminder name
   - **Message**: Additional notes
   - **Time**: When to be reminded
   - **Recurrence**: How often to repeat
3. Optionally link to an existing task
4. Click **Create Reminder**

### Tracking Progress
1. Visit **Stats** section
2. View your **Activity Heatmap**
3. Check **Summary**, **Daily**, **Weekly**, or **Monthly** reports
4. Monitor your streak and completion rates

---

## 🏆 XP & Achievement System

### XP Rewards
| Action | XP Reward |
|--------|-----------|
| Complete Low Priority Task | +10 XP |
| Complete Medium Priority Task | +20 XP |
| Complete High Priority Task | +50 XP |
| Complete a Project | +100 XP |
| Finish Pomodoro Session | +5 XP |
| Daily Streak Bonus | +10 XP × days |

### Level Titles
Progress through levels to earn titles:
- **Level 1-5**: Beginner → Apprentice → Practitioner
- **Level 6-10**: Achiever → Expert → Master
- **Level 11+**: Grandmaster → Legend

---

## ⚙️ Build Options

```bash
# Standard build
make

# Debug build (with symbols)
make debug

# Release build (optimized)
make release

# Clean build files
make clean

# Show build info
make info

# Display help
make help
```

---

## 🔌 API Endpoints

The web server exposes RESTful endpoints:

### Tasks
- `GET /api/tasks` - List all tasks
- `POST /api/tasks/create` - Create a task
- `POST /api/tasks/update` - Update a task
- `POST /api/tasks/delete` - Delete a task
- `POST /api/tasks/complete` - Toggle completion
- `POST /api/tasks/assign` - Assign to project

### Projects
- `GET /api/projects` - List all projects
- `POST /api/projects/create` - Create a project
- `POST /api/projects/update` - Update a project
- `POST /api/projects/delete` - Delete a project

### Reminders
- `GET /api/reminders` - List all reminders
- `GET /api/reminders/pending` - Get pending reminders
- `GET /api/reminders/today` - Get today's reminders
- `POST /api/reminders/create` - Create a reminder
- `POST /api/reminders/update` - Update a reminder
- `POST /api/reminders/delete` - Delete a reminder
- `POST /api/reminders/reschedule` - Reschedule a reminder

### Pomodoro
- `GET /api/pomodoro/state` - Get timer state
- `POST /api/pomodoro/start` - Start work session
- `POST /api/pomodoro/break` - Start short break
- `POST /api/pomodoro/longbreak` - Start long break
- `POST /api/pomodoro/complete` - Mark session complete
- `POST /api/pomodoro/stop` - Stop/abandon session

### XP & Achievements
- `GET /api/xp` - Get XP status
- `GET /api/achievements` - List achievements
- `POST /api/achievements/update` - Update achievement

### Statistics
- `GET /api/stats/summary` - Get summary stats
- `GET /api/stats/daily` - Get daily report
- `GET /api/stats/weekly` - Get weekly report
- `GET /api/stats/monthly` - Get monthly report
- `GET /api/stats/heatmap` - Get heatmap data

---

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- Inspired by gamification principles from apps like Duolingo and Habitica
- GitHub-style activity heatmap visualization
- Modern glassmorphism design trends

---

<div align="center">

**Made with ❤️ for productivity enthusiasts**

⭐ Star this repo if you find it useful!

</div>
