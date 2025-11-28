/**
 * @file TaskController.h
 * @brief Controller for task operations - bridges QML actions to C++ logic
 */

#ifndef TASKCONTROLLER_H
#define TASKCONTROLLER_H

#include <QObject>
#include <QString>
#include <memory>

class TaskManager;
class XPSystem;

/**
 * @brief Controller for task-related operations
 * 
 * Provides Q_INVOKABLE methods that can be called from QML
 * to perform task operations (CRUD, completion, etc.)
 */
class TaskController : public QObject
{
    Q_OBJECT

public:
    explicit TaskController(QObject *parent = nullptr);
    ~TaskController() override;

    /**
     * @brief Create a new task
     * @param title Task title
     * @param description Task description
     * @param priority Priority level (0-2)
     * @param dueDate Due date string (YYYY-MM-DD)
     * @param projectId Associated project ID (0 for none)
     * @return Created task ID, or -1 on failure
     */
    Q_INVOKABLE int createTask(const QString& title, 
                                const QString& description = "",
                                int priority = 1,
                                const QString& dueDate = "",
                                int projectId = 0);

    /**
     * @brief Update an existing task
     */
    Q_INVOKABLE bool updateTask(int taskId, 
                                 const QString& title,
                                 const QString& description,
                                 int priority);

    /**
     * @brief Delete a task
     */
    Q_INVOKABLE bool deleteTask(int taskId);

    /**
     * @brief Complete a task and award XP
     * @return XP awarded
     */
    Q_INVOKABLE int completeTask(int taskId);

    /**
     * @brief Add a pomodoro to a task
     */
    Q_INVOKABLE bool addPomodoro(int taskId);

    /**
     * @brief Get task count statistics
     */
    Q_INVOKABLE int getTaskCount() const;
    Q_INVOKABLE int getCompletedTaskCount() const;
    Q_INVOKABLE double getCompletionRate() const;

signals:
    void taskCreated(int taskId);
    void taskUpdated(int taskId);
    void taskDeleted(int taskId);
    void taskCompleted(int taskId, int xpGained);
    void pomodoroAdded(int taskId, int totalPomodoros);
    void errorOccurred(const QString& message);

private:
    std::unique_ptr<TaskManager> m_taskManager;
    std::unique_ptr<XPSystem> m_xpSystem;
};

#endif // TASKCONTROLLER_H
