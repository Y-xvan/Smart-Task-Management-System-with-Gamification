## Local Web UI plan (no Qt, no extra installs)

Goal: provide a browser-based pop-out UI without installing Qt/SDL/ImGui. The C++ app embeds a minimal HTTP server (std::thread + sockets), auto-opens the default browser, and serves a modern HTML/CSS/JS SPA. All backend capabilities are reused through JSON endpoints.

### Backend surface (must keep all existing features)
- Tasks: list/create/update/delete, complete, priority, due date, tags, pomodoro counters, assign to project.
- Projects: list/create/update/delete, color, target date, archive toggle, progress and task listing.
- Reminders: list/create/delete/reschedule, pending/today views, recurrence (once/daily/weekly/monthly), optional task link.
- Achievements & XP: current level/title, XP gain, achievement progress/unlock.
- Pomodoro: start work / short break / long break; update cycles and task pomodoros.
- Statistics & heatmap: daily/weekly/monthly reports, streaks, totals, 90-day heatmap data.

### Server endpoints (to implement)
- `GET /api/tasks`, `POST /api/tasks`, `PATCH /api/tasks/{id}`, `DELETE /api/tasks/{id}`, `POST /api/tasks/{id}/complete`, `POST /api/tasks/{id}/assign`.
- `GET /api/projects`, `POST /api/projects`, `PATCH /api/projects/{id}`, `DELETE /api/projects/{id}`.
- `GET /api/reminders`, `POST /api/reminders`, `PATCH /api/reminders/{id}`, `DELETE /api/reminders/{id}`, `GET /api/reminders/today`, `GET /api/reminders/pending`.
- `GET /api/xp`, `POST /api/xp/award` (internal use), `GET /api/achievements`.
- `GET /api/pomodoro/state`, `POST /api/pomodoro/start`, `POST /api/pomodoro/break`, `POST /api/pomodoro/longbreak`.
- `GET /api/stats/summary`, `GET /api/stats/daily`, `GET /api/stats/weekly`, `GET /api/stats/monthly`, `GET /api/stats/heatmap`.

### UI composition (SPA)
- **Dashboard (home):** hero banner, streak flame, XP bar, quick stats cards (tasks due today, reminders today, pomodoros today, achievements unlocked).
- **Tasks board:** Kanban columns (Todo/In Progress/Done), sortable by priority/due date; modal for create/edit with color tags; inline complete; project badge.
- **Projects gallery:** grid cards with color label, progress bar, target date; drill-in to task list and stats.
- **Reminders:** agenda list for today + upcoming; recurrence chips; reschedule inline.
- **Pomodoro focus:** big circular timer, start/break buttons, task selector, progress ring animation.
- **Achievements/XP:** badge wall, progress meters, level ladder.
- **Statistics:** sparkline/mini charts (JS), 90-day heatmap grid.

### Visual & interaction cues
- Dark theme with accent gradient; glassmorphism panels; micro-animations on hover/click; soft shadows; emoji/status chips for fun.
- Responsive layout (single-column on narrow, two/three-column on wide); sticky navbar with sections.

### Delivery steps
1) Guard out Qt in `main.cpp` (build succeeds without Qt), keep console path intact.
2) Add minimal HTTP server and JSON serializer (no external deps) plus auto-open browser on `http://127.0.0.1:8787`.
3) Implement the REST endpoints by delegating to existing managers; ensure DB init and integrity flow stays unchanged.
4) Serve static assets (HTML/CSS/JS) from in-repo `web/` folder; build a single-page app with fetch calls to endpoints.
5) Provide a default landing page that exercises all endpoints (tasks/projects/reminders/pomodoro/XP/stats).
6) Keep console UI usable via flag (`--console`), default to web UI when browser is available.

No placeholder text like “开发中”：all exposed features mirror existing backend behavior. 
