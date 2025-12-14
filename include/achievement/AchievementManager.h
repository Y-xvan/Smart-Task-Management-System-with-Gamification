#pragma once
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "../database/DAO/AchievementDAO.h"
#include "../statistics/StatisticsAnalyzer.h"
#include "../common/entities.h"  // 包含实体定义

// 成就进度信息
struct AchievementProgress {
    int achievementId;
    int currentProgress;
    int targetProgress;
    double progressPercent;
};

// 成就条件检查器，负责根据用户行为自动判定成就解锁与进度
class AchievementChecker {
public:
    AchievementChecker() = default;

    // 检查所有待解锁成就
    void checkAllAchievements(int userId);

    // 检查特定类型的成就
    void checkTaskAchievements(int userId, int taskCount);
    void checkStreakAchievements(int userId, int streakDays);
    void checkPomodoroAchievements(int userId, int pomodoroCount);
};

class AchievementManager {
private:
    std::unique_ptr<AchievementDAO> achievementDAO;
    std::unique_ptr<StatisticsAnalyzer> statisticsAnalyzer;
    int currentUserId;
    
    // 成就定义缓存
    std::vector<Achievement> achievementDefinitions;
    std::unordered_map<std::string, Achievement> userAchievements;
    
public:
    AchievementManager(std::unique_ptr<AchievementDAO> dao, int userId = 1);
    
    // 核心方法
    void initialize();
    void checkAllAchievements();
    void unlockAchievement(const std::string& achievementId);

    // 成就进度核心方法
    // 旧接口，基于字符串成就ID 的进度更新（用于兼容已有代码）
    void updateAchievementProgress(const std::string& achievementId, int progress);

    // 新接口，基于 userId + achievementId 的进度更新与递增
    void updateAchievementProgress(int userId, int achievementId, int newValue);
    void incrementAchievementProgress(int userId, int achievementId, int increment);

    // 获取指定用户的所有成就进度，用于 UI 显示进度条和百分比
    std::vector<AchievementProgress> getAchievementProgress(int userId);

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
    void printAchievement(const Achievement& definition,
                         const Achievement* userAchievement = nullptr) const;

    // 缓存工具方法
    const Achievement* findAchievementDefinition(const std::string& key) const;
    Achievement* findUserAchievement(const std::string& key);
    bool isAchievementUnlocked(const std::string& key) const;
    std::string getAchievementKeyById(int achievementId) const;
    void refreshUserAchievementCache(const std::vector<Achievement>& entries);
    
    // 统计信息获取方法（需要队友实现其他DAO）
    int getCompletedTaskCount() const;
    int getCurrentStreak() const;
    int getDailyTaskCount(const std::string& date) const;
    int getTotalPomodoroCount() const;

    void setStatisticsAnalyzer(std::unique_ptr<StatisticsAnalyzer> customAnalyzer);
    
    void setCurrentUserId(int userId);
    int getCurrentUserId() const;

    // 供 Web/UI 编辑使用
    const Achievement* getDefinitionById(int id) const;
    bool updateAchievementDefinition(int id,
                                     const std::string& name,
                                     const std::string& description,
                                     int targetValue);
    int createAchievementDefinition(const std::string& name,
                                    const std::string& description,
                                    const std::string& unlockCondition,
                                    int targetValue,
                                    int rewardXP = 100,
                                    const std::string& category = "custom",
                                    const std::string& icon = "🏆");
};
