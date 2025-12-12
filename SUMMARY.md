## Backend capabilities

- **Database manager**: `DatabaseManager` handles SQLite connection, initialization, integrity checks, and table existence verification for `task_manager.db`.
- **DAO layer**: `TaskDAOImpl`, `ProjectDAO`, `ReminderDAO`, `AchievementDAO` provide CRUD/persistence for tasks, projects, reminders, achievements, plus user stats/settings.
- **Task management** (`TaskManager`/`Task`): create/update/delete, priority, due dates, tags, completion state, pomodoro counters, project assignment, list & lookup helpers.
- **Project management** (`ProjectManager`/`Project`): create/update/delete, color labels, target dates, archive toggle, progress computed from task completion, task listings per project.
- **Reminder system** (`ReminderSystem`): create active reminders with recurrence (once/daily/weekly/monthly), pending/today views, reschedule/delete, task linkage.
- **Achievements & XP** (`AchievementManager`, `XPSystem`): XP awarding, level/title calculation, progress queries, checking/unlocking achievements, streak tracking.
- **Pomodoro timer** (`Pomodoro`): work/short/long break timers with countdown callbacks, cycle tracking, stats integration.
- **Statistics & visualization** (`StatisticsAnalyzer`, `HeatmapVisualizer`): daily/weekly/monthly reports, summary/streak/XP stats, 90-day completion heatmap generation.
- **Console UI coverage**: UIManager menus drive every manager above (tasks, projects, reminders, pomodoro, stats, XP/achievements), with selection helpers, validation, and gamified feedback.

## Current UI coverage

- **Console UI** (`UIManager`): colorful, menu-driven flows covering all backend areas—task CRUD/completion, project CRUD/details, reminder creation/listing/reschedule/delete, pomodoro sessions with countdown and XP rewards, statistics/heatmap display, achievements/XP dashboards, and gamified HUD/messages.
- **Startup** (`main.cpp`): animated console intro, database integrity checks, then launches either GUI (Qt/QML) or the console UI. Console path is fully wired to managers above.
- **QML resources** (`resources/qml/*.qml`): modern mobile-style screens (AppLauncher, MainView with tab bar for quests/projects/focus/stats/reminders, dialogs/cards). These are not currently hooked to C++ backend types and rely on a `gameController` object that is not exposed yet.

## Gap to a pop-out GUI

- **Missing Qt toolchain**: `main.cpp` depends on `QApplication/QQmlApplicationEngine`, so builds fail without Qt dev packages installed.
- **Link configuration**: Makefile does not link QtQuick/QtWidgets libraries, so even with headers present the GUI binary would not link.
- **Resource deployment**: QML files exist but are not packaged (no qrc/bundle step) in the current build.
- **Expose backend data**: publish tasks/projects/reminders/XP stats to QML via `QObject` properties, `QQmlContext`, or list models.
- **Bridge & actions**: implement the `gameController` QObject, register list models for cards, and wire QML actions back to TaskManager/ProjectManager/ReminderSystem/AchievementManager.
- **Fallback behavior**: console UI remains the working path; GUI cannot run until the Qt toolchain and C++/QML bridge are added.

## Next-step recommendations

1) Make Qt optional in the build (macro-guard GUI path) so console builds succeed while developing the bridge.  
2) Introduce a `GameController` QObject exposing XP/streak and Task/Project/Reminder list models to QML; connect actions for create/update/complete.  
3) Add Qt build flags/resource bundling to Makefile or migrate to CMake for easier Qt integration; verify QML loads from `resources/qml`.  
4) Iterate on QML screens with real data bindings and signal-slot wiring for add/edit dialogs, then re-enable GUI launch by default when Qt is available.  
