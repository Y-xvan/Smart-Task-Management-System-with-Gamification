#ifndef PROJECT_DAO_H
#define PROJECT_DAO_H

#include "common/Entities.h"
#include <vector>
#include <optional>
#include <chrono>

struct ProjectStats {
    int totalTasks;
    int completedTasks;
    int pendingTasks;
    double completionRate;
    int totalPomodoros;
    std::string createdDate;
    std::string lastActivity;
    
    ProjectStats() 
        : totalTasks(0), completedTasks(0), pendingTasks(0), 
          completionRate(0.0), totalPomodoros(0) {}
};

class ProjectDAO {
public:
    virtual ~ProjectDAO() = default;
    
    // 基础CRUD操作
    virtual bool insertProject(Project& project) = 0;
    virtual bool updateProject(const Project& project) = 0;
    virtual bool deleteProject(int projectId) = 0;
    virtual bool softDeleteProject(int projectId) = 0;
    
    // 查询操作
    virtual std::optional<Project> getProjectById(int projectId) = 0;
    virtual std::vector<Project> getAllProjects() = 0;
    virtual std::vector<Project> getActiveProjects() = 0;
    virtual std::vector<Project> getArchivedProjects() = 0;
    virtual std::vector<Project> getCompletedProjects() = 0;
    virtual std::vector<Project> getProjectsByColor(const std::string& colorCode) = 0;
    
    // 进度管理
    virtual bool updateProjectProgress(int projectId) = 0;
    virtual bool updateTaskCounts(int projectId, int totalTasks, int completedTasks) = 0;
    virtual double calculateProjectProgress(int projectId) = 0;
    
    // 任务关联管理
    virtual bool addTaskToProject(int taskId, int projectId) = 0;
    virtual bool removeTaskFromProject(int taskId, int projectId) = 0;
    virtual bool moveTaskToProject(int taskId, int newProjectId) = 0;
    virtual std::vector<int> getTaskIdsInProject(int projectId) = 0;
    virtual int getTaskCountInProject(int projectId) = 0;
    virtual int getCompletedTaskCountInProject(int projectId) = 0;
    
    // 统计查询
    virtual std::vector<Project> getProjectsByCompletionRate(double minRate, double maxRate) = 0;
    virtual ProjectStats getProjectStatistics(int projectId) = 0;
    virtual int getTotalProjectCount() = 0;
    virtual int getActiveProjectCount() = 0;
    
    // 数据验证
    virtual bool projectExists(int projectId) = 0;
    virtual bool isProjectNameUnique(const std::string& name, int excludeId = 0) = 0;
};

#endif // PROJECT_DAO_H

