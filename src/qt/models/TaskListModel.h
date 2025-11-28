/**
 * @file TaskListModel.h
 * @brief QAbstractListModel for Task data - bridges C++ TaskManager to QML
 * 
 * Provides a list model that exposes Task objects to QML for display
 * in ListView, GridView, or Repeater components.
 */

#ifndef TASKLISTMODEL_H
#define TASKLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <memory>

// Forward declarations
class TaskController;
class TaskManager;

/**
 * @brief Task data structure for QML binding
 */
struct TaskItem {
    int id = 0;
    QString title;
    QString description;
    int priority = 1;      // 0: Low, 1: Medium, 2: High
    QString dueDate;
    bool completed = false;
    QString tags;
    int projectId = 0;
    int pomodoroCount = 0;
    int estimatedPomodoros = 0;
    QString completedDate;
};

/**
 * @brief QAbstractListModel implementation for Task list
 * 
 * Roles:
 * - IdRole: Task ID
 * - TitleRole: Task title
 * - DescriptionRole: Task description
 * - PriorityRole: Priority level (0-2)
 * - PriorityColorRole: Color string based on priority
 * - DueDateRole: Due date string
 * - CompletedRole: Completion status
 * - TagsRole: Comma-separated tags
 * - PomodoroCountRole: Completed pomodoros
 * - EstimatedPomodorosRole: Estimated pomodoros
 * - PomodoroProgressRole: Progress ratio (0.0 - 1.0)
 */
class TaskListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int completedCount READ completedCount NOTIFY completedCountChanged)
    Q_PROPERTY(int pendingCount READ pendingCount NOTIFY pendingCountChanged)

public:
    enum TaskRoles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        DescriptionRole,
        PriorityRole,
        PriorityColorRole,
        PriorityIconRole,
        DueDateRole,
        CompletedRole,
        TagsRole,
        ProjectIdRole,
        PomodoroCountRole,
        EstimatedPomodorosRole,
        PomodoroProgressRole
    };

    explicit TaskListModel(QObject *parent = nullptr);
    ~TaskListModel() override;

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Property accessors
    int count() const;
    int completedCount() const;
    int pendingCount() const;

    // Controller connection
    void setController(TaskController* controller);

public slots:
    /**
     * @brief Refresh model data from database
     */
    void refresh();

    /**
     * @brief Get task by ID (for QML)
     */
    Q_INVOKABLE QVariantMap getTask(int id) const;

    /**
     * @brief Filter tasks by completion status
     * @param showCompleted Include completed tasks
     * @param showPending Include pending tasks
     */
    Q_INVOKABLE void setFilter(bool showCompleted, bool showPending);

signals:
    void countChanged();
    void completedCountChanged();
    void pendingCountChanged();
    void taskCompleted(int taskId, int xpGained);
    void errorOccurred(const QString& message);

private:
    QList<TaskItem> m_tasks;
    
    // Controller relationship:
    // - m_controller: External controller for UI-triggered actions (signals for model updates)
    // - m_taskManager: Internal data source for loading tasks
    // The controller connects to this model to receive notifications when data changes.
    // The model uses its own TaskManager instance for data loading to avoid circular dependencies.
    TaskController* m_controller = nullptr;
    std::unique_ptr<TaskManager> m_taskManager;
    
    bool m_showCompleted = true;
    bool m_showPending = true;

    void loadTasks();
    QString priorityToColor(int priority) const;
    QString priorityToIcon(int priority) const;
};

#endif // TASKLISTMODEL_H
