#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <string>
#include <thread>
#include <atomic>
#include <functional>

#include "task/TaskManager.h"
#include "project/ProjectManager.h"
#include "reminder/ReminderSystem.h"
#include "gamification/XPSystem.h"
#include "statistics/StatisticsAnalyzer.h"
#include "achievement/AchievementManager.h"
#include "Pomodoro/pomodoro.h"
#include "HeatmapVisualizer/HeatmapVisualizer.h"

/**
 * A minimal embedded HTTP server (no external deps) that serves a Web UI and JSON APIs.
 * Note: Designed for local use (127.0.0.1), single-threaded request handling.
 */
class WebServer {
public:
    WebServer(int port,
              const std::string& staticDir,
              TaskManager* taskMgr,
              ProjectManager* projMgr,
              ReminderSystem* reminderSys,
              XPSystem* xpSys,
              StatisticsAnalyzer* stats,
              AchievementManager* achieve,
              Pomodoro* pomodoro,
              HeatmapVisualizer* heatmap);
    ~WebServer();

    void start();
    void stop();

private:
    int port;
    std::string staticDir;
    TaskManager* taskMgr;
    ProjectManager* projMgr;
    ReminderSystem* reminderSys;
    XPSystem* xpSys;
    StatisticsAnalyzer* stats;
    AchievementManager* achieve;
    Pomodoro* pomodoro;
    HeatmapVisualizer* heatmap;

    std::thread serverThread;
    std::atomic<bool> running;
    std::thread pomoThread;
    std::atomic<bool> pomoRunning{false};

    void run();
    void handleClient(int clientSock);

    std::string handleRequest(const std::string& method,
                              const std::string& path,
                              const std::string& body,
                              int& status,
                              std::string& contentType);

    // API helpers
    std::string jsonTasks();
    std::string jsonOverdueTasks();
    std::string jsonTodayTasks();
    std::string jsonProjects();
    std::string jsonReminders();
    std::string jsonRemindersToday();
    std::string jsonRemindersPending();
    std::string jsonXP();
    std::string jsonAchievements();
    std::string jsonStatsSummary();
    std::string jsonStatsDaily();
    std::string jsonStatsWeekly();
    std::string jsonStatsMonthly();
    std::string jsonStatsHeatmap();

    static std::string readFile(const std::string& path);
    static bool fileExists(const std::string& path);

    static std::string urlDecode(const std::string& str);
    static std::unordered_map<std::string, std::string> parseQuery(const std::string& path);
    std::string okJson(const std::string& msg = "ok");
    std::string errorJson(const std::string& msg);
    void stopPomodoroThread();
};

#endif
