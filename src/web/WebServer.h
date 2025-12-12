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
              StatisticsAnalyzer* stats);
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

    std::thread serverThread;
    std::atomic<bool> running;

    void run();
    void handleClient(int clientSock);

    std::string handleRequest(const std::string& method,
                              const std::string& path,
                              const std::string& body,
                              int& status,
                              std::string& contentType);

    // API helpers
    std::string jsonTasks();
    std::string jsonProjects();
    std::string jsonReminders();

    static std::string readFile(const std::string& path);
    static bool fileExists(const std::string& path);
};

#endif
