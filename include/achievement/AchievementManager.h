#pragma once
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "AchievementDAO.h"
#include "Entities.h"  // 包含实体定义

class AchievementManager {
private:
    std::unique_ptr<AchievementDAO> achievementDAO;
    int currentUserId;
    
    // 成就定义缓存
    std::vector<AchievementDefinition> achievementDefinitions;
    std::unordered_map<std::string, UserAchievement> userAchievements;
    
public:
    AchievementManager(std::unique_ptr<AchievementDAO> dao, int userId = 1);
    
    // 核心方法
    void initialize();
    void checkAllAchievements();
    void unlockAchievement(const std::string& achievementId);
    void updateAchievementProgress(const std::string& achievementId, int progress);
    void displayUnlockedAchievements();
    void displayAllAchievements();
    void displayAchievementStatistics();
    
    // 具体成就检查方法
    void checkFirstTaskAchievement();
    void checkSevenDayStreakAchievement();
    void checkTimeManagementAchievement();
    void checkPomodoroMasterAchievement();
    
    // 工具方法
    bool loadAchievementDefinitions();
    bool loadUserAchievements();
    void printAchievement(const AchievementDefinition& definition, 
                         const UserAchievement* userAchievement = nullptr) const;
    
    // 统计信息获取方法（需要队友实现其他DAO）
    int getCompletedTaskCount() const;
    int getCurrentStreak() const;
    int getDailyTaskCount(const std::string& date) const;
    int getTotalPomodoroCount() const;
    
    void setCurrentUserId(int userId);
    int getCurrentUserId() const;
};
