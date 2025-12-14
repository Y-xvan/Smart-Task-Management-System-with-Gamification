#include "database/DAO/AchievementDAO.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <filesystem>


using namespace std;

AchievementDAO::AchievementDAO() : dataFilePath("./data/") {
    std::filesystem::create_directories(dataFilePath);
    resetAllDataIfVersionChanged();
    initializeDefaultAchievements();
}

AchievementDAO::AchievementDAO(const std::string& basePath) : dataFilePath(basePath) {
    if (!dataFilePath.empty() && dataFilePath.back() != '/') {
        dataFilePath += '/';
    }

    std::filesystem::create_directories(dataFilePath);
    resetAllDataIfVersionChanged();
    initializeDefaultAchievements();
}

std::string AchievementDAO::getDefinitionFilePath() const {
    return dataFilePath + "achievement_definitions.csv";
}

std::string AchievementDAO::getUserAchievementFilePath(int userId) const {
    return dataFilePath + "user_achievements_" + std::to_string(userId) + ".csv";
}

bool AchievementDAO::fileExists(const std::string& filename) const {
    std::ifstream file(filename);
    return file.good();
}

bool AchievementDAO::writeFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    return true;
}

std::vector<Achievement> AchievementDAO::buildDefaultAchievementTemplates() {
    std::vector<Achievement> templates;
    
    auto addTemplate = [&templates](const std::string& name,
                                    const std::string& description,
                                    const std::string& icon,
                                    const std::string& unlockKey,
                                    int rewardXP,
                                    const std::string& category,
                                    int targetValue) {
        Achievement achievement;
        achievement.id = 0;  // Will be assigned later
        achievement.name = name;
        achievement.description = description;
        achievement.icon = icon;
        achievement.unlock_condition = unlockKey;
        achievement.unlocked = false;
        achievement.reward_xp = rewardXP;
        achievement.category = category;
        achievement.progress = 0;
        achievement.target_value = targetValue;
        templates.push_back(achievement);
    };
    
    // === Task Completion Achievements (1→5→10→25→50→100→200) ===
    addTemplate("First Task", "Complete your first task", "🎯", "task_1", 100, "task", 1);
    addTemplate("Task Beginner", "Complete 5 tasks", "📝", "task_5", 150, "task", 5);
    addTemplate("Task Learner", "Complete 10 tasks", "📋", "task_10", 200, "task", 10);
    addTemplate("Task Achiever", "Complete 25 tasks", "⭐", "task_25", 300, "task", 25);
    addTemplate("Task Expert", "Complete 50 tasks", "🌟", "task_50", 500, "task", 50);
    addTemplate("Task Master", "Complete 100 tasks", "💫", "task_100", 800, "task", 100);
    addTemplate("Task Legend", "Complete 200 tasks", "👑", "task_200", 1500, "task", 200);

    // === Project Completion Achievements (1→5→10→25→50→100→200) ===
    // Note: These achievements require project completion tracking to be implemented
    addTemplate("First Project", "Complete your first project", "📁", "project_1", 150, "special", 1);
    addTemplate("Project Starter", "Complete 5 projects", "📂", "project_5", 250, "special", 5);
    addTemplate("Project Builder", "Complete 10 projects", "🏗️", "project_10", 400, "special", 10);
    addTemplate("Project Manager", "Complete 25 projects", "📊", "project_25", 600, "special", 25);
    addTemplate("Project Director", "Complete 50 projects", "🎯", "project_50", 1000, "special", 50);
    addTemplate("Project Executive", "Complete 100 projects", "🏆", "project_100", 1500, "special", 100);
    addTemplate("Project Titan", "Complete 200 projects", "👑", "project_200", 2500, "special", 200);

    // === Streak Achievements (1→5→10→25→50→100→200 days) ===
    addTemplate("First Day", "Complete tasks for 1 day", "📅", "streak_1", 50, "streak", 1);
    addTemplate("Getting Started", "Maintain a 5-day streak", "🔥", "streak_5", 150, "streak", 5);
    addTemplate("Consistency", "Maintain a 10-day streak", "🔥", "streak_10", 300, "streak", 10);
    addTemplate("Habit Forming", "Maintain a 25-day streak", "💪", "streak_25", 500, "streak", 25);
    addTemplate("Dedicated", "Maintain a 50-day streak", "⚡", "streak_50", 800, "streak", 50);
    addTemplate("Unstoppable", "Maintain a 100-day streak", "🌟", "streak_100", 1500, "streak", 100);
    addTemplate("Legendary Streak", "Maintain a 200-day streak", "👑", "streak_200", 3000, "streak", 200);

    // === Pomodoro Achievements (1→5→10→25→50→100→200 sessions) ===
    addTemplate("First Pomodoro", "Complete your first Pomodoro session", "🍅", "pomodoro_1", 50, "time", 1);
    addTemplate("Pomodoro Beginner", "Complete 5 Pomodoro sessions", "🍅", "pomodoro_5", 100, "time", 5);
    addTemplate("Pomodoro Learner", "Complete 10 Pomodoro sessions", "🍅", "pomodoro_10", 200, "time", 10);
    addTemplate("Pomodoro Practitioner", "Complete 25 Pomodoro sessions", "⏱️", "pomodoro_25", 350, "time", 25);
    addTemplate("Pomodoro Expert", "Complete 50 Pomodoro sessions", "⏰", "pomodoro_50", 600, "time", 50);
    addTemplate("Pomodoro Master", "Complete 100 Pomodoro sessions", "🎯", "pomodoro_100", 1000, "time", 100);
    addTemplate("Pomodoro Legend", "Complete 200 Pomodoro sessions", "👑", "pomodoro_200", 2000, "time", 200);
    
    return templates;
}

void AchievementDAO::initializeDefaultAchievements() {
    // 如果已经有定义文件并且能加载成功，就不再初始化默认成就
    if (!loadAchievementDefinitions()) {
        const std::string now = getCurrentTimestamp();
        
        // Use the shared template builder to avoid duplication
        std::vector<Achievement> templates = buildDefaultAchievementTemplates();
        
        for (auto& achievement : templates) {
            achievement.id = generateAchievementId();
            achievement.created_date = now;
            achievement.updated_date = now;
            achievement.unlocked_date.clear();
            achievementDefinitions.push_back(achievement);
        }

        saveAchievementDefinitions();
    }
}

bool AchievementDAO::loadAchievementDefinitions() {
    ifstream file(getDefinitionFilePath());
    if (!file.is_open()) {
        return false;
    }
    
    achievementDefinitions.clear();
    string line;
    getline(file, line); // 跳过标题行
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string id, created_date, updated_date, name, description, icon;
        string unlock_condition, unlocked_str, unlocked_date, reward_xp_str;
        string category, progress_str, target_value_str;
        
        getline(ss, id, ',');
        getline(ss, created_date, ',');
        getline(ss, updated_date, ',');
        getline(ss, name, ',');
        getline(ss, description, ',');
        getline(ss, icon, ',');
        getline(ss, unlock_condition, ',');
        getline(ss, unlocked_str, ',');
        getline(ss, unlocked_date, ',');
        getline(ss, reward_xp_str, ',');
        getline(ss, category, ',');
        getline(ss, progress_str, ',');
        getline(ss, target_value_str, ',');
        
        Achievement achievement;
        achievement.id = stoi(id);
        achievement.created_date = created_date;
        achievement.updated_date = updated_date;
        achievement.name = name;
        achievement.description = description;
        achievement.icon = icon;
        achievement.unlock_condition = unlock_condition;
        achievement.unlocked = (unlocked_str == "1");
        achievement.unlocked_date = unlocked_date;
        achievement.reward_xp = stoi(reward_xp_str);
        achievement.category = category;
        achievement.progress = stoi(progress_str);
        achievement.target_value = stoi(target_value_str);
        
        achievementDefinitions.push_back(achievement);
        nextAchievementId = std::max(nextAchievementId, achievement.id + 1);
    }
    
    file.close();
    return true;
}

bool AchievementDAO::saveAchievementDefinitions() {
    ofstream file(getDefinitionFilePath());
    if (!file.is_open()) {
        return false;
    }
    
    file << "id,created_date,updated_date,name,description,icon,unlock_condition,"
            "unlocked,unlocked_date,reward_xp,category,progress,target_value\n";
    
    for (const auto& achievement : achievementDefinitions) {
        file << achievement.id << ","
             << achievement.created_date << ","
             << achievement.updated_date << ","
             << achievement.name << ","
             << achievement.description << ","
             << achievement.icon << ","
             << achievement.unlock_condition << ","
             << (achievement.unlocked ? "1" : "0") << ","
             << achievement.unlocked_date << ","
             << achievement.reward_xp << ","
             << achievement.category << ","
             << achievement.progress << ","
             << achievement.target_value << "\n";
    }
    
    file.close();
    return true;
}

bool AchievementDAO::updateAchievementDefinition(int id,
                                                 const std::string& name,
                                                 const std::string& description,
                                                 int targetValue) {
    for (auto& achievement : achievementDefinitions) {
        if (achievement.id == id) {
            if (!name.empty()) achievement.name = name;
            if (!description.empty()) achievement.description = description;
            if (targetValue > 0) achievement.target_value = targetValue;
            achievement.updated_date = getCurrentTimestamp();
            return saveAchievementDefinitions();
        }
    }
    return false;
}

int AchievementDAO::createAchievementDefinition(const std::string& name,
                                                const std::string& description,
                                                const std::string& unlockCondition,
                                                int targetValue,
                                                int rewardXP,
                                                const std::string& category,
                                                const std::string& icon) {
    const std::string now = getCurrentTimestamp();
    
    Achievement achievement;
    achievement.id = generateAchievementId();
    achievement.created_date = now;
    achievement.updated_date = now;
    achievement.name = name;
    achievement.description = description;
    achievement.icon = icon;
    achievement.unlock_condition = unlockCondition;
    achievement.unlocked = false;
    achievement.unlocked_date.clear();
    achievement.reward_xp = rewardXP;
    achievement.category = category;
    achievement.progress = 0;
    achievement.target_value = targetValue;
    
    achievementDefinitions.push_back(achievement);
    
    if (saveAchievementDefinitions()) {
        return achievement.id;
    }
    return -1;
}

vector<Achievement> AchievementDAO::getAllAchievementDefinitions() const {
    return achievementDefinitions;
}

Achievement AchievementDAO::getAchievementDefinition(const string& achievementKey) const {
    for (const auto& achievement : achievementDefinitions) {
        if (achievement.unlock_condition == achievementKey) {
            return achievement;
        }
    }
    return Achievement();
}

bool AchievementDAO::loadUserAchievements(int userId) {
    ifstream file(getUserAchievementFilePath(userId));
    if (!file.is_open()) {
        // 如果用户文件不存在，基于成就定义生成初始成就数据
        if (achievementDefinitions.empty()) {
            // 尝试先加载定义，如果仍然为空则初始化默认定义
            loadAchievementDefinitions();
            if (achievementDefinitions.empty()) {
                initializeDefaultAchievements();
            }
        }

        const std::string now = getCurrentTimestamp();
        userAchievements.clear();
        for (const auto& definition : achievementDefinitions) {
            Achievement userEntry = definition;
            userEntry.id = generateAchievementId();
            userEntry.created_date = now;
            userEntry.updated_date = now;
            userEntry.unlocked = false;
            userEntry.unlocked_date.clear();
            userEntry.progress = 0;
            userAchievements.push_back(userEntry);
        }

        return saveUserAchievements(userId);
    }
    
    userAchievements.clear();
    string line;
    getline(file, line); // 跳过标题行
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string id, created_date, updated_date, name, description, icon;
        string unlock_condition, unlocked_str, unlocked_date, reward_xp_str;
        string category, progress_str, target_value_str;
        
        getline(ss, id, ',');
        getline(ss, created_date, ',');
        getline(ss, updated_date, ',');
        getline(ss, name, ',');
        getline(ss, description, ',');
        getline(ss, icon, ',');
        getline(ss, unlock_condition, ',');
        getline(ss, unlocked_str, ',');
        getline(ss, unlocked_date, ',');
        getline(ss, reward_xp_str, ',');
        getline(ss, category, ',');
        getline(ss, progress_str, ',');
        getline(ss, target_value_str, ',');
        
        Achievement achievement;
        achievement.id = stoi(id);
        achievement.created_date = created_date;
        achievement.updated_date = updated_date;
        achievement.name = name;
        achievement.description = description;
        achievement.icon = icon;
        achievement.unlock_condition = unlock_condition;
        achievement.unlocked = (unlocked_str == "1");
        achievement.unlocked_date = unlocked_date;
        achievement.reward_xp = stoi(reward_xp_str);
        achievement.category = category;
        achievement.progress = stoi(progress_str);
        achievement.target_value = stoi(target_value_str);
        
        userAchievements.push_back(achievement);
        nextAchievementId = std::max(nextAchievementId, achievement.id + 1);
    }
    
    file.close();
    return true;
}

bool AchievementDAO::saveUserAchievements(int userId) {
    ofstream file(getUserAchievementFilePath(userId));
    if (!file.is_open()) {
        return false;
    }
    
    file << "id,created_date,updated_date,name,description,icon,unlock_condition,"
            "unlocked,unlocked_date,reward_xp,category,progress,target_value\n";
    
    for (const auto& achievement : userAchievements) {
        file << achievement.id << ","
             << achievement.created_date << ","
             << achievement.updated_date << ","
             << achievement.name << ","
             << achievement.description << ","
             << achievement.icon << ","
             << achievement.unlock_condition << ","
             << (achievement.unlocked ? "1" : "0") << ","
             << achievement.unlocked_date << ","
             << achievement.reward_xp << ","
             << achievement.category << ","
             << achievement.progress << ","
             << achievement.target_value << "\n";
    }
    
    file.close();
    return true;
}

vector<Achievement> AchievementDAO::getUserAchievements(int userId) const {
    (void)userId; // 当前实现中依赖先调用 loadUserAchievements(userId)
    return userAchievements;
}

Achievement* AchievementDAO::getUserAchievement(int userId, const string& achievementKey) {
    (void)userId; // 实际上依赖于调用者先 loadUserAchievements(userId)
    for (auto& achievement : userAchievements) {
        if (achievement.unlock_condition == achievementKey) {
            return &achievement;
        }
    }
    
    Achievement definition = getAchievementDefinition(achievementKey);
    if (definition.id == 0) {
        return nullptr;
    }
    
    Achievement newAchievement = definition;
    newAchievement.id = generateAchievementId();
    newAchievement.created_date = getCurrentTimestamp();
    newAchievement.updated_date = getCurrentTimestamp();
    newAchievement.unlocked = false;
    newAchievement.unlocked_date = "";
    newAchievement.progress = 0;
    
    userAchievements.push_back(newAchievement);
    return &userAchievements.back();
}

bool AchievementDAO::unlockAchievement(int userId, const string& achievementKey) {
    Achievement* achievement = getUserAchievement(userId, achievementKey);
    if (achievement && !achievement->unlocked) {
        achievement->unlocked = true;
        achievement->progress = achievement->target_value;
        achievement->unlocked_date = getCurrentTimestamp();
        achievement->updated_date = getCurrentTimestamp();
        return saveUserAchievements(userId);
    }
    return false;
}

bool AchievementDAO::updateAchievementProgress(int userId, const string& achievementKey, int progress) {
    Achievement* achievement = getUserAchievement(userId, achievementKey);
    if (achievement && !achievement->unlocked) {
        achievement->progress = progress;
        achievement->updated_date = getCurrentTimestamp();
        
        if (progress >= achievement->target_value) {
            return unlockAchievement(userId, achievementKey);
        }
        
        return saveUserAchievements(userId);
    }
    return false;
}

bool AchievementDAO::resetUserAchievements(int userId) {
    userAchievements.clear();
    return saveUserAchievements(userId);
}

int AchievementDAO::getUnlockedAchievementCount(int userId) const {
    (void)userId; // 依赖调用者先 loadUserAchievements(userId)
    int count = 0;
    for (const auto& achievement : userAchievements) {
        if (achievement.unlocked) count++;
    }
    return count;
}

int AchievementDAO::getTotalXP(int userId) const {
    (void)userId; // 依赖调用者先 loadUserAchievements(userId)
    int totalXP = 0;
    for (const auto& achievement : userAchievements) {
        if (achievement.unlocked) totalXP += achievement.reward_xp;
    }
    return totalXP;
}

std::vector<Achievement> AchievementDAO::getRecentlyUnlockedAchievements(int userId, int count) const {
    (void)userId; // 同样依赖之前已 loadUserAchievements(userId)
    std::vector<Achievement> unlocked;
    for (const auto& achievement : userAchievements) {
        if (achievement.unlocked) {
            unlocked.push_back(achievement);
        }
    }

    std::sort(unlocked.begin(), unlocked.end(),
              [](const Achievement& a, const Achievement& b) {
                  return a.unlocked_date > b.unlocked_date; // 依赖时间字符串格式可比较
              });

    if (count > 0 && static_cast<int>(unlocked.size()) > count) {
        unlocked.resize(count);
    }
    return unlocked;
}

bool AchievementDAO::userAchievementFileExists(int userId) const {
    return fileExists(getUserAchievementFilePath(userId));
}

std::string AchievementDAO::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm tm{};
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

int AchievementDAO::generateAchievementId() {
    return nextAchievementId++;
}

std::string AchievementDAO::getVersionFilePath() const {
    return dataFilePath + "achievement_version.txt";
}

std::string AchievementDAO::computeDefinitionVersionHash() const {
    // Create a deterministic hash based on the default achievement definitions.
    // This hash will change whenever the code-defined achievements change.
    // We use the shared template builder to avoid duplication.
    
    std::vector<Achievement> templates = buildDefaultAchievementTemplates();
    
    // Build a canonical string representation of all default achievements
    std::ostringstream oss;
    for (const auto& ach : templates) {
        // Include all relevant fields that define an achievement
        oss << ach.unlock_condition << ":"
            << ach.name << ":"
            << ach.description << ":"
            << ach.icon << ":"
            << ach.reward_xp << ":"
            << ach.category << ":"
            << ach.target_value << "|";
    }
    
    std::string content = oss.str();
    
    // Simple hash function (djb2)
    unsigned long hash = 5381;
    for (char c : content) {
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
    }
    
    std::ostringstream hashStr;
    hashStr << std::hex << hash;
    return hashStr.str();
}

std::string AchievementDAO::loadStoredVersionHash() const {
    std::ifstream file(getVersionFilePath());
    if (!file.is_open()) {
        return "";  // No version file exists
    }
    
    std::string storedHash;
    std::getline(file, storedHash);
    // Using RAII - file is automatically closed when destructor is called
    return storedHash;
}

bool AchievementDAO::saveVersionHash(const std::string& hash) {
    std::ofstream file(getVersionFilePath(), std::ios::trunc);
    if (!file.is_open()) {
        return false;
    }
    file << hash;
    // Using RAII - file is automatically flushed and closed when destructor is called
    return true;
}

bool AchievementDAO::isVersionChanged() const {
    std::string currentHash = computeDefinitionVersionHash();
    std::string storedHash = loadStoredVersionHash();
    
    // If no stored hash, this is a fresh install - not a version change
    if (storedHash.empty()) {
        return false;
    }
    
    return currentHash != storedHash;
}

void AchievementDAO::resetAllDataIfVersionChanged() {
    // Compute hash and load stored hash once to avoid redundant computation
    std::string currentHash = computeDefinitionVersionHash();
    std::string storedHash = loadStoredVersionHash();
    
    bool versionChanged = !storedHash.empty() && (currentHash != storedHash);
    
    if (!versionChanged) {
        // Version is the same or first run
        if (storedHash.empty()) {
            // First run, save the current version hash
            saveVersionHash(currentHash);
        }
        return;
    }
    
    std::cout << "检测到成就系统版本变化，正在重置成就数据...\n";
    
    // Delete old data files
    std::string defPath = getDefinitionFilePath();
    
    // Remove achievement definitions file
    std::filesystem::remove(defPath);
    
    // Remove all user achievement files (user_achievements_*.csv)
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dataFilePath)) {
            std::string filename = entry.path().filename().string();
            if (filename.find("user_achievements_") == 0 && 
                filename.rfind(".csv") == filename.size() - 4) {
                std::filesystem::remove(entry.path());
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "清理用户成就文件时出错: " << e.what() << "\n";
    }
    
    // Clear in-memory data
    achievementDefinitions.clear();
    userAchievements.clear();
    nextAchievementId = 1;
    
    // Save the new version hash (reuse currentHash computed at start of function)
    saveVersionHash(currentHash);
    
    std::cout << "成就数据已重置为新版本\n";
}
