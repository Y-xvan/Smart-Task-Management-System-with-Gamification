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
#include <filesystem>

using namespace std;

namespace {
    constexpr int BUFFER_SIZE = 8192;

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
                     StatisticsAnalyzer* stats)
    : port(port),
      staticDir(staticDir),
      taskMgr(taskMgr),
      projMgr(projMgr),
      reminderSys(reminderSys),
      xpSys(xpSys),
      stats(stats),
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
    if (path == "/api/tasks" && method == "GET") {
        contentType = "application/json";
        return jsonTasks();
    }
    if (path == "/api/projects" && method == "GET") {
        contentType = "application/json";
        return jsonProjects();
    }
    if (path == "/api/reminders" && method == "GET") {
        contentType = "application/json";
        return jsonReminders();
    }

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
