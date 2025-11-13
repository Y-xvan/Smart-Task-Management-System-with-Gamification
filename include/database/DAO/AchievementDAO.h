#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "common/entities.h"  // 使用统一的实体定义

class AchievementDAO {
private:
    std::string dataFilePath;
    
    // 内部数据存储
    std::vector<Achievement> achievementDefinitions;
    std::vector<Achievement> userAchievements;  // 使用统一的Achievement实体
    
    // 文件操作
    bool loadFromFile();
    bool saveToFile();
    std::string getDefinitionFilePath() const;
    std::string getUserAchievementFilePath(int userId) const;

public:
    AchievementDAO();
    explicit AchievementDAO(const std::string& basePath);
    
    // 成就定义管理
    bool loadAchievementDefinitions();
    std::vector<Achievement> getAllAchievementDefinitions() const;
    Achievement getAchievementDefinition(const std::string& achievementName) const;
    Achievement getAchievementDefinitionById(int achievementId) const;
    
    // 用户成就管理
    bool loadUserAchievements(int userId);
    bool saveUserAchievements(int userId);
    std::vector<Achievement> getUserAchievements(int userId) const;
    Achievement* getUserAchievement(int userId, const std::string& achievementName);
    Achievement* getUserAchievementById(int userId, int achievementId);
    
    // 成就进度更新
    bool unlockAchievement(int userId, const std::string& achievementName);
    bool unlockAchievementById(int userId, int achievementId);
    bool updateAchievementProgress(int userId, const std::string& achievementName, int progress);
    bool updateAchievementProgressById(int userId, int achievementId, int progress);
    bool resetUserAchievements(int userId);
    
    // 统计查询
    int getUnlockedAchievementCount(int userId) const;
    int getTotalXP(int userId) const;
    std::vector<Achievement> getRecentlyUnlockedAchievements(int userId, int count = 5) const;
    std::vector<Achievement> getAchievementsByCategory(int userId, const std::string& category) const;
    
    // 工具方法
    void initializeDefaultAchievements();
    bool userAchievementFileExists(int userId) const;
    
    // 生成时间戳
    static std::string getCurrentTimestamp();
    
private:
    // ID生成器
    int generateAchievementId();
    int generateUserAchievementId();
    int nextAchievementId = 1;
    int nextUserAchievementId = 1;
};
