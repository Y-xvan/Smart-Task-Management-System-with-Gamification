/**
 * @file TaskController.cpp
 * @brief Implementation of TaskController
 */

#include "TaskController.h"
#include "task/TaskManager.h"
#include "task/task.h"
#include "gamification/XPSystem.h"

TaskController::TaskController(QObject *parent)
    : QObject(parent)
    , m_taskManager(std::make_unique<TaskManager>())
    , m_xpSystem(std::make_unique<XPSystem>())
{
    m_taskManager->initialize();
}

TaskController::~TaskController() = default;

int TaskController::createTask(const QString& title, 
                                const QString& description,
                                int priority,
                                const QString& dueDate,
                                int projectId)
{
    if (title.trimmed().isEmpty()) {
        emit errorOccurred("Task title cannot be empty");
        return -1;
    }

    Task task(title.toStdString(), description.toStdString(), projectId);
    // Note: priority and dueDate would be set if the Task class supports them
    
    int taskId = m_taskManager->createTask(task);
    
    if (taskId > 0) {
        emit taskCreated(taskId);
    } else {
        emit errorOccurred("Failed to create task");
    }
    
    return taskId;
}

bool TaskController::updateTask(int taskId, 
                                 const QString& title,
                                 const QString& description,
                                 int priority)
{
    auto taskOpt = m_taskManager->getTask(taskId);
    if (!taskOpt) {
        emit errorOccurred("Task not found");
        return false;
    }

    Task& task = taskOpt.value();
    task.setName(title.toStdString());
    task.setDescription(description.toStdString());
    // Note: setPriority would be called if the Task class supports it

    bool success = m_taskManager->updateTask(task);
    
    if (success) {
        emit taskUpdated(taskId);
    } else {
        emit errorOccurred("Failed to update task");
    }
    
    return success;
}

bool TaskController::deleteTask(int taskId)
{
    bool success = m_taskManager->deleteTask(taskId);
    
    if (success) {
        emit taskDeleted(taskId);
    } else {
        emit errorOccurred("Failed to delete task");
    }
    
    return success;
}

int TaskController::completeTask(int taskId)
{
    auto taskOpt = m_taskManager->getTask(taskId);
    if (!taskOpt) {
        emit errorOccurred("Task not found");
        return 0;
    }

    if (taskOpt->isCompleted()) {
        emit errorOccurred("Task already completed");
        return 0;
    }

    bool success = m_taskManager->completeTask(taskId);
    
    if (success) {
        // Award XP based on priority (default to medium = 1)
        int priority = 1;
        int xpReward = m_xpSystem->getXPForTaskCompletion(priority);
        m_xpSystem->awardXP(xpReward, "Task completed");
        
        emit taskCompleted(taskId, xpReward);
        return xpReward;
    } else {
        emit errorOccurred("Failed to complete task");
        return 0;
    }
}

bool TaskController::addPomodoro(int taskId)
{
    bool success = m_taskManager->addPomodoro(taskId);
    
    if (success) {
        int totalPomodoros = m_taskManager->getPomodoroCount(taskId);
        
        // Award XP for pomodoro
        int xpReward = m_xpSystem->getXPForPomodoro();
        m_xpSystem->awardXP(xpReward, "Pomodoro completed");
        
        emit pomodoroAdded(taskId, totalPomodoros);
    } else {
        emit errorOccurred("Failed to add pomodoro");
    }
    
    return success;
}

int TaskController::getTaskCount() const
{
    return m_taskManager->getTaskCount();
}

int TaskController::getCompletedTaskCount() const
{
    return m_taskManager->getCompletedTaskCount();
}

double TaskController::getCompletionRate() const
{
    return m_taskManager->getCompletionRate();
}
