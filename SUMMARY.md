## Backend capabilities

- **SQLite-backed data layer** via `DatabaseManager` and DAO classes (`TaskDAOImpl`, `ProjectDAO`, `ReminderDAO`, `AchievementDAO`) for tasks, projects, reminders, achievements, user stats/settings, integrity checks, and table existence verification.
- **Task management** (`TaskManager`/`Task`): create/update/delete, priority, due dates, tags, completion state, pomodoro counters, project assignment, list & lookup helpers.
- **Project management** (`ProjectManager`/`Project`): create/update/delete, color labels, target dates, archive toggle, progress computed from task completion, task listings per project.
- **Reminder system** (`ReminderSystem`): create active reminders with recurrence (once/daily/weekly/monthly), pending/today views, reschedule/delete, task linkage.
- **Achievements & XP** (`AchievementManager`, `XPSystem`): XP awarding, level/title calculation, progress queries, checking/unlocking achievements, streak tracking.
- **Pomodoro timer** (`Pomodoro`): work/short/long break timers with countdown callbacks, cycle tracking, stats integration.
- **Statistics & visualization** (`StatisticsAnalyzer`, `HeatmapVisualizer`): daily/weekly/monthly reports, summary/streak/XP stats, 90-day completion heatmap generation.

## Current UI coverage

- **Console UI** (`UIManager`): colorful, menu-driven flows covering all backend areas—task CRUD/completion, project CRUD/details, reminder creation/listing/reschedule/delete, pomodoro sessions with countdown and XP rewards, statistics/heatmap display, achievements/XP dashboards, and gamified HUD/messages.
- **Startup** (`main.cpp`): animated console intro, database integrity checks, then launches either GUI (Qt/QML) or the console UI. Console path is fully wired to managers above.
- **QML resources** (`resources/qml/*.qml`): modern mobile-style screens (AppLauncher, MainView with tab bar for quests/projects/focus/stats/reminders, dialogs/cards). These are not currently hooked to C++ backend types and rely on a `gameController` object that is not exposed yet.

## Gap to a pop-out GUI

- **Build blockers**: `main.cpp` includes Qt headers (`QApplication`, `QQmlApplicationEngine`, etc.), so the project fails to compile without Qt dev packages; Makefile also lacks Qt link flags. QML files are present but not packaged/loaded in this environment.
- **Integration work needed**: expose backend data/models (tasks, projects, reminders, XP/achievement stats) to QML via `QObject`/`QQmlContext` or model types, implement the `gameController` bridge, and register models for lists/cards. Wire QML actions back to TaskManager/ProjectManager/etc.
- **Packaging**: add Qt to the toolchain, update the build to link QtQuick/QtWidgets, and ensure QML resources are deployed (qrc or install alongside binary).
- **Fallback behavior**: console UI remains the working path; GUI cannot run until the Qt toolchain and C++/QML bridge are added.

## Next-step recommendations

1) Make Qt optional in the build (macro-guard GUI path) so console builds succeed while developing the bridge.  
2) Introduce a `GameController` QObject exposing XP/streak and Task/Project/Reminder list models to QML; connect actions for create/update/complete.  
3) Add Qt build flags/resource bundling to Makefile or migrate to CMake for easier Qt integration; verify QML loads from `resources/qml`.  
4) Iterate on QML screens with real data bindings and signal-slot wiring for add/edit dialogs, then re-enable GUI launch by default when Qt is available.  
