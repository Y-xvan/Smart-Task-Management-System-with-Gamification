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
    initializeDefaultAchievements();
}

AchievementDAO::AchievementDAO(const std::string& basePath) : dataFilePath(basePath) {
    if (!dataFilePath.empty() && dataFilePath.back() != '/') {
        dataFilePath += '/';
    }

    std::filesystem::create_directories(dataFilePath);
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

void AchievementDAO::initializeDefaultAchievements() {
    // 如果已经有定义文件并且能加载成功，就不再初始化默认成就
    if (!loadAchievementDefinitions()) {
        const std::string now = getCurrentTimestamp();

        auto addDefinition = [&](const std::string& name,
                                 const std::string& description,
                                 const std::string& icon,
                                 const std::string& unlockKey,
                                 int rewardXP,
                                 const std::string& category,
                                 int targetValue) {
            Achievement achievement;
            achievement.id = generateAchievementId();
            achievement.created_date = now;
            achievement.updated_date = now;
            achievement.name = name;
            achievement.description = description;
            achievement.icon = icon;
            achievement.unlock_condition = unlockKey;
            achievement.unlocked = false;
            achievement.unlocked_date.clear();
            achievement.reward_xp = rewardXP;
            achievement.category = category;
            achievement.progress = 0;
            achievement.target_value = targetValue;
            achievementDefinitions.push_back(achievement);
        };

        // 与新成就系统对应的默认成就
        addDefinition("首次任务", "完成第一个任务", "🎯", "first_task", 100, "task", 1);
        addDefinition("七日连胜", "连续完成7天任务", "🔥", "seven_day_streak", 300, "streak", 7);
        addDefinition("时间管理达人", "单日完成10个任务", "⏱️", "time_management_master", 200, "time", 10);
        addDefinition("番茄钟大师", "累计完成20个番茄钟", "🍅", "pomodoro_master", 250, "pomodoro", 20);

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
