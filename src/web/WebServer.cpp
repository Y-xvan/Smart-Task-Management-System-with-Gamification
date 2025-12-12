#include "web/WebServer.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include "HeatmapVisualizer/HeatmapVisualizer.h"
#include <filesystem>
#include <unordered_map>
#include <chrono>
#include <thread>

using namespace std;

namespace {
    constexpr int BUFFER_SIZE = 8192;
    string escape(const string& s){
        string out;
        for(char c: s){
            if(c=='"') out+="\\\"";
            else if(c=='\\') out+="\\\\";
            else if(c=='\n') out+="\\n";
            else out+=c;
        }
        return out;
    }

    int createServerSocket(int port) {
        int sock = ::socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) return -1;
        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (::bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
            ::close(sock);
            return -1;
        }
        if (::listen(sock, 10) < 0) {
            ::close(sock);
            return -1;
        }
        return sock;
    }

    vector<string> split(const string& s, char delim) {
        vector<string> tokens;
        string token;
        stringstream ss(s);
        while (getline(ss, token, delim)) tokens.push_back(token);
        return tokens;
    }
}

WebServer::WebServer(int port,
                     const std::string& staticDir,
                     TaskManager* taskMgr,
                     ProjectManager* projMgr,
                     ReminderSystem* reminderSys,
                     XPSystem* xpSys,
                     StatisticsAnalyzer* stats,
                     AchievementManager* achieve,
                     Pomodoro* pomodoro,
                     HeatmapVisualizer* heatmap)
    : port(port),
      staticDir(staticDir),
      taskMgr(taskMgr),
      projMgr(projMgr),
      reminderSys(reminderSys),
      xpSys(xpSys),
      stats(stats),
      achieve(achieve),
      pomodoro(pomodoro),
      heatmap(heatmap),
      running(false) {}

WebServer::~WebServer() {
    stop();
}

void WebServer::start() {
    if (running.load()) return;
    running = true;
    serverThread = std::thread(&WebServer::run, this);
}

void WebServer::stop() {
    running = false;
    stopPomodoroThread();
    if (serverThread.joinable()) serverThread.join();
}

void WebServer::run() {
    int serverSock = createServerSocket(port);
    if (serverSock < 0) {
        cerr << "[WebServer] Failed to start server on port " << port << endl;
        return;
    }
    cout << "[WebServer] Listening on http://127.0.0.1:" << port << endl;

    while (running.load()) {
        sockaddr_in clientAddr{};
        socklen_t len = sizeof(clientAddr);
        int clientSock = ::accept(serverSock, reinterpret_cast<sockaddr*>(&clientAddr), &len);
        if (clientSock < 0) continue;
        handleClient(clientSock);
        ::close(clientSock);
    }
    ::close(serverSock);
}

void WebServer::handleClient(int clientSock) {
    char buffer[BUFFER_SIZE] = {0};
    int received = ::recv(clientSock, buffer, BUFFER_SIZE - 1, 0);
    if (received <= 0) return;
    string request(buffer, received);

    stringstream ss(request);
    string line;
    getline(ss, line);
    string method, path, version;
    {
        stringstream l(line);
        l >> method >> path >> version;
    }

    string headers;
    string body;
    bool inBody = false;
    while (getline(ss, line)) {
        if (line == "\r" || line == "") {
            inBody = true;
            continue;
        }
        if (inBody) {
            body += line + "\n";
        } else {
            headers += line + "\n";
        }
    }

    int status = 200;
    string contentType = "text/html; charset=utf-8";
    string respBody = handleRequest(method, path, body, status, contentType);

    stringstream resp;
    resp << "HTTP/1.1 " << status << " OK\r\n";
    resp << "Content-Type: " << contentType << "\r\n";
    resp << "Content-Length: " << respBody.size() << "\r\n";
    resp << "Connection: close\r\n\r\n";
    resp << respBody;

    auto respStr = resp.str();
    ::send(clientSock, respStr.c_str(), respStr.size(), 0);
}

std::string WebServer::handleRequest(const std::string& method,
                                     const std::string& path,
                                     const std::string& body,
                                     int& status,
                                     std::string& contentType) {
    if (path == "/" || path == "/index.html") {
        auto fp = staticDir + "/index.html";
        if (fileExists(fp)) return readFile(fp);
    }
    if (path.rfind("/static/", 0) == 0) {
        auto fp = staticDir + path;
        if (fileExists(fp)) {
            if (fp.find(".css") != string::npos) contentType = "text/css; charset=utf-8";
            else if (fp.find(".js") != string::npos) contentType = "application/javascript";
            else contentType = "text/plain; charset=utf-8";
            return readFile(fp);
        }
        status = 404; return "Not Found";
    }

    // API endpoints
    // Tasks
    if (path.rfind("/api/tasks", 0) == 0) {
        contentType = "application/json";
        auto q = parseQuery(path);
        if (path == "/api/tasks" && method == "GET") return jsonTasks();
        if (path == "/api/tasks/overdue" && method == "GET") return jsonOverdueTasks();
        if (path == "/api/tasks/today" && method == "GET") return jsonTodayTasks();
        if (path.rfind("/api/tasks/create", 0) == 0 && method == "POST") {
            Task t(q["name"], q["desc"]);
            if (q.count("priority")) t.setPriority(stoi(q["priority"]));
            if (q.count("due")) t.setDueDate(q["due"]);
            if (q.count("tags")) t.setTags(q["tags"]);
            if (q.count("projectId")) t.setProjectId(stoi(q["projectId"]));
            if (q.count("estPomodoro")) t.setEstimatedPomodoros(stoi(q["estPomodoro"]));
            int id = taskMgr->createTask(t);
            return okJson("created:" + to_string(id));
        }
        if (path.rfind("/api/tasks/update", 0) == 0 && method == "POST") {
            int id = stoi(q["id"]);
            auto opt = taskMgr->getTask(id);
            if (!opt.has_value()) return errorJson("not found");
            auto task = opt.value();
            if (q.count("name")) task.setName(q["name"]);
            if (q.count("desc")) task.setDescription(q["desc"]);
            if (q.count("priority")) task.setPriority(stoi(q["priority"]));
            if (q.count("due")) task.setDueDate(q["due"]);
            if (q.count("tags")) task.setTags(q["tags"]);
            if (q.count("completed")) task.setCompleted(q["completed"] == "true");
            if (q.count("projectId")) task.setProjectId(stoi(q["projectId"]));
            if (taskMgr->updateTask(task)) return okJson(); else return errorJson("update failed");
        }
        if (path.rfind("/api/tasks/delete", 0) == 0 && method == "POST") {
            int id = stoi(q["id"]);
            if (taskMgr->deleteTask(id)) return okJson(); else return errorJson("delete failed");
        }
        if (path.rfind("/api/tasks/complete", 0) == 0 && method == "POST") {
            int id = stoi(q["id"]);
            if (taskMgr->completeTask(id)) {
                int prio = 1;
                auto t = taskMgr->getTask(id);
                if (t.has_value()) prio = t->getPriority();
                int xp = xpSys->getXPForTaskCompletion(prio);
                xpSys->awardXP(xp, "complete task");
                achieve->checkAllAchievements();
                return okJson();
            }
            return errorJson("complete failed");
        }
        if (path.rfind("/api/tasks/assign", 0) == 0 && method == "POST") {
            int id = stoi(q["id"]);
            int pid = stoi(q["projectId"]);
            if (taskMgr->assignTaskToProject(id, pid)) return okJson(); else return errorJson("assign failed");
        }
        if (path.rfind("/api/tasks/pomodoro", 0) == 0 && method == "POST") {
            int id = stoi(q["id"]);
            if (taskMgr->addPomodoro(id)) return okJson(); else return errorJson("pomodoro failed");
        }
    }

    // Projects
    if (path.rfind("/api/projects", 0) == 0) {
        contentType = "application/json";
        auto q = parseQuery(path);
        if (path == "/api/projects" && method == "GET") return jsonProjects();
        if (path.rfind("/api/projects/create", 0) == 0 && method == "POST") {
            Project p(q["name"], q["desc"], q.count("color") ? q["color"] : "#4CAF50");
            if (q.count("target")) p.setTargetDate(q["target"]);
            int id = projMgr->createProject(p);
            return okJson("created:" + to_string(id));
        }
        if (path.rfind("/api/projects/update", 0) == 0 && method == "POST") {
            int id = stoi(q["id"]);
            Project* p = projMgr->getProject(id);
            if (!p) return errorJson("not found");
            if (q.count("name")) p->setName(q["name"]);
            if (q.count("desc")) p->setDescription(q["desc"]);
            if (q.count("color")) p->setColorLabel(q["color"]);
            if (q.count("target")) p->setTargetDate(q["target"]);
            if (projMgr->updateProject(*p)) return okJson(); else return errorJson("update failed");
        }
        if (path.rfind("/api/projects/delete", 0) == 0 && method == "POST") {
            int id = stoi(q["id"]);
            if (projMgr->deleteProject(id)) return okJson(); else return errorJson("delete failed");
        }
    }

    // Reminders
    if (path.rfind("/api/reminders", 0) == 0) {
        contentType = "application/json";
        auto q = parseQuery(path);
        if (path == "/api/reminders" && method == "GET") return jsonReminders();
        if (path == "/api/reminders/today" && method == "GET") return jsonRemindersToday();
        if (path == "/api/reminders/pending" && method == "GET") return jsonRemindersPending();
        if (path.rfind("/api/reminders/create", 0) == 0 && method == "POST") {
            reminderSys->addReminder(q["title"], q["message"], q["time"], q["recurrence"], q.count("taskId") ? stoi(q["taskId"]) : 0);
            return okJson();
        }
        if (path.rfind("/api/reminders/reschedule", 0) == 0 && method == "POST") {
            int id = stoi(q["id"]);
            if (reminderSys->rescheduleReminder(id, q["time"])) return okJson(); else return errorJson("reschedule failed");
        }
        if (path.rfind("/api/reminders/delete", 0) == 0 && method == "POST") {
            int id = stoi(q["id"]);
            if (reminderSys->deleteReminder(id)) return okJson(); else return errorJson("delete failed");
        }
        if (path == "/api/reminders/check" && method == "POST") {
            reminderSys->checkDueReminders();
            return okJson();
        }
    }

    // Pomodoro
    if (path.rfind("/api/pomodoro", 0) == 0) {
        contentType = "application/json";
        auto q = parseQuery(path);
        if (path == "/api/pomodoro/state" && method == "GET") {
            stringstream ss; ss << "{";
            ss << "\"running\":" << (pomoRunning.load() ? "true" : "false") << ",";
            ss << "\"cycles\":" << pomodoro->getCycleCount();
            ss << "}";
            return ss.str();
        }
        auto startThread = [&](auto fn) {
            stopPomodoroThread();
            pomoRunning = true;
            pomoThread = std::thread([&, fn]() {
                fn();
                pomoRunning = false;
            });
        };
        if (path == "/api/pomodoro/start" && method == "POST") {
            startThread([&]() { pomodoro->startWork(); });
            return okJson();
        }
        if (path == "/api/pomodoro/break" && method == "POST") {
            startThread([&]() { pomodoro->startBreak(); });
            return okJson();
        }
        if (path == "/api/pomodoro/longbreak" && method == "POST") {
            startThread([&]() { pomodoro->startLongBreak(); });
            return okJson();
        }
        if (path == "/api/pomodoro/stop" && method == "POST") {
            pomodoro->stop();
            stopPomodoroThread();
            return okJson();
        }
    }

    // XP & achievements
    if (path == "/api/xp" && method == "GET") {
        contentType = "application/json";
        return jsonXP();
    }
    if (path == "/api/achievements" && method == "GET") {
        contentType = "application/json";
        return jsonAchievements();
    }

    // Stats
    if (path == "/api/stats/summary" && method == "GET") { contentType="application/json"; return jsonStatsSummary(); }
    if (path == "/api/stats/daily" && method == "GET") { contentType="application/json"; return jsonStatsDaily(); }
    if (path == "/api/stats/weekly" && method == "GET") { contentType="application/json"; return jsonStatsWeekly(); }
    if (path == "/api/stats/monthly" && method == "GET") { contentType="application/json"; return jsonStatsMonthly(); }
    if (path == "/api/stats/heatmap" && method == "GET") { contentType="application/json"; return jsonStatsHeatmap(); }

    status = 404;
    return "Not Found";
}

std::string WebServer::jsonTasks() {
    auto tasks = taskMgr->getAllTasks();
    stringstream ss;
    ss << "[";
    for (size_t i = 0; i < tasks.size(); ++i) {
        const auto& t = tasks[i];
        ss << "{"
           << "\"id\":" << t.getId() << ","
           << "\"name\":\"" << t.getName() << "\","
           << "\"desc\":\"" << t.getDescription() << "\","
           << "\"completed\":" << (t.isCompleted() ? "true" : "false") << ","
           << "\"priority\":" << t.getPriority() << ","
           << "\"due\":\"" << t.getDueDate() << "\""
           << "}";
        if (i + 1 < tasks.size()) ss << ",";
    }
    ss << "]";
    return ss.str();
}

std::string WebServer::jsonOverdueTasks() {
    auto tasks = taskMgr->getOverdueTasks();
    stringstream ss; ss << "[";
    for (size_t i=0;i<tasks.size();++i){
        const auto& t=tasks[i];
        ss<<"{\"id\":"<<t.getId()<<",\"name\":\""<<t.getName()<<"\",\"due\":\""<<t.getDueDate()<<"\"}";
        if(i+1<tasks.size()) ss<<",";
    }
    ss<<"]"; return ss.str();
}

std::string WebServer::jsonTodayTasks() {
    auto tasks = taskMgr->getTodayTasks();
    stringstream ss; ss << "[";
    for (size_t i=0;i<tasks.size();++i){
        const auto& t=tasks[i];
        ss<<"{\"id\":"<<t.getId()<<",\"name\":\""<<t.getName()<<"\",\"due\":\""<<t.getDueDate()<<"\"}";
        if(i+1<tasks.size()) ss<<",";
    }
    ss<<"]"; return ss.str();
}

std::string WebServer::jsonProjects() {
    auto projects = projMgr->getAllProjects();
    stringstream ss;
    ss << "[";
    for (size_t i = 0; i < projects.size(); ++i) {
        auto p = projects[i];
        ss << "{"
           << "\"id\":" << p->getId() << ","
           << "\"name\":\"" << p->getName() << "\","
           << "\"progress\":" << p->getProgress()
           << "}";
        if (i + 1 < projects.size()) ss << ",";
    }
    ss << "]";
    return ss.str();
}

std::string WebServer::jsonReminders() {
    auto reminders = reminderSys->getActiveReminders();
    stringstream ss;
    ss << "[";
    for (size_t i = 0; i < reminders.size(); ++i) {
        const auto& r = reminders[i];
        ss << "{"
           << "\"id\":" << r.id << ","
           << "\"title\":\"" << r.title << "\","
           << "\"time\":\"" << r.trigger_time << "\","
           << "\"recurrence\":\"" << r.recurrence << "\""
           << "}";
        if (i + 1 < reminders.size()) ss << ",";
    }
    ss << "]";
    return ss.str();
}

std::string WebServer::jsonRemindersToday() {
    auto reminders = reminderSys->getDueRemindersForToday();
    stringstream ss; ss<<"[";
    for(size_t i=0;i<reminders.size();++i){
        const auto& r=reminders[i];
        ss<<"{\"id\":"<<r.id<<",\"title\":\""<<r.title<<"\",\"time\":\""<<r.trigger_time<<"\"}";
        if(i+1<reminders.size()) ss<<",";
    }
    ss<<"]"; return ss.str();
}

std::string WebServer::jsonRemindersPending() {
    auto all = reminderSys->getActiveReminders();
    vector<Reminder> pending;
    for (auto& r: all) if (!r.triggered && r.enabled) pending.push_back(r);
    stringstream ss; ss<<"[";
    for(size_t i=0;i<pending.size();++i){
        const auto& r=pending[i];
        ss<<"{\"id\":"<<r.id<<",\"title\":\""<<r.title<<"\",\"time\":\""<<r.trigger_time<<"\"}";
        if(i+1<pending.size()) ss<<",";
    }
    ss<<"]"; return ss.str();
}

std::string WebServer::jsonXP() {
    stringstream ss; ss<<"{";
    ss<<"\"level\":"<<xpSys->getCurrentLevel()<<",";
    ss<<"\"xp\":"<<xpSys->getCurrentXP()<<",";
    ss<<"\"next\":"<<xpSys->getXPForNextLevel()<<",";
    ss<<"\"title\":\""<<xpSys->getCurrentLevelTitle()<<"\"";
    ss<<"}";
    return ss.str();
}

std::string WebServer::jsonAchievements() {
    auto list = achieve->getAchievementProgress(1);
    stringstream ss; ss<<"[";
    for(size_t i=0;i<list.size();++i){
        const auto& a=list[i];
        ss<<"{\"id\":"<<a.achievementId<<",\"progress\":"<<a.currentProgress<<",\"target\":"<<a.targetProgress<<",\"percent\":"<<a.progressPercent<<"}";
        if(i+1<list.size()) ss<<",";
    }
    ss<<"]"; return ss.str();
}

std::string WebServer::jsonStatsSummary() {
    stringstream ss; ss<<"{";
    ss<<"\"tasksCompleted\":"<<stats->getTotalTasksCompleted()<<",";
    ss<<"\"completionRate\":"<<stats->getCompletionRate()<<",";
    ss<<"\"streak\":"<<stats->getCurrentStreak()<<",";
    ss<<"\"longestStreak\":"<<stats->getLongestStreak()<<",";
    ss<<"\"pomodoros\":"<<stats->getTotalPomodoros()<<",";
    ss<<"\"pomodorosToday\":"<<stats->getPomodorosToday();
    ss<<"}";
    return ss.str();
}

std::string WebServer::jsonStatsDaily() {
    stringstream ss; ss<<"{\"report\":\""<<escape(stats->generateDailyReport())<<"\"}";
    return ss.str();
}
std::string WebServer::jsonStatsWeekly() {
    stringstream ss; ss<<"{\"report\":\""<<escape(stats->generateWeeklyReport())<<"\"}";
    return ss.str();
}
std::string WebServer::jsonStatsMonthly() {
    stringstream ss; ss<<"{\"report\":\""<<escape(stats->generateMonthlyReport())<<"\"}";
    return ss.str();
}
std::string WebServer::jsonStatsHeatmap() {
    stringstream ss; ss<<"{\"heatmap\":\""<<escape(heatmap->generateHeatmap(90))<<"\"}";
    return ss.str();
}

bool WebServer::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

std::string WebServer::readFile(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) return "";
    std::ostringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

std::string WebServer::urlDecode(const std::string& str) {
    std::string ret;
    char ch;
    int i, ii;
    for (i=0; i<str.length(); i++) {
        if (int(str[i]) == 37) {
            sscanf(str.substr(i+1,2).c_str(), "%x", &ii);
            ch=static_cast<char>(ii);
            ret+=ch;
            i=i+2;
        } else if (str[i]=='+') ret+=' ';
        else ret+=str[i];
    }
    return ret;
}

std::unordered_map<std::string, std::string> WebServer::parseQuery(const std::string& path) {
    unordered_map<string,string> m;
    auto pos = path.find('?');
    if (pos == string::npos) return m;
    auto q = path.substr(pos+1);
    stringstream ss(q);
    string kv;
    while (getline(ss, kv, '&')) {
        auto eq = kv.find('=');
        if (eq == string::npos) continue;
        auto k = kv.substr(0, eq);
        auto v = kv.substr(eq+1);
        m[k] = urlDecode(v);
    }
    return m;
}

std::string WebServer::okJson(const std::string& msg) {
    stringstream ss; ss<<"{\"status\":\"ok\"";
    if (!msg.empty()) ss<<",\"message\":\""<<msg<<"\"";
    ss<<"}";
    return ss.str();
}
std::string WebServer::errorJson(const std::string& msg) {
    stringstream ss; ss<<"{\"status\":\"error\",\"message\":\""<<msg<<"\"}";
    return ss.str();
}

void WebServer::stopPomodoroThread() {
    if (pomoThread.joinable()) {
        pomodoro->stop();
        pomoThread.join();
    }
    pomoRunning = false;
}
