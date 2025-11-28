/**
 * @file TaskListModel.cpp
 * @brief Implementation of TaskListModel
 */

#include "TaskListModel.h"
#include "task/TaskManager.h"
#include "controllers/TaskController.h"

TaskListModel::TaskListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_taskManager(std::make_unique<TaskManager>())
{
    m_taskManager->initialize();
    loadTasks();
}

TaskListModel::~TaskListModel() = default;

int TaskListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_tasks.size());
}

QVariant TaskListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_tasks.size()))
        return QVariant();

    const TaskItem &task = m_tasks.at(index.row());

    switch (role) {
    case IdRole:
        return task.id;
    case TitleRole:
        return task.title;
    case DescriptionRole:
        return task.description;
    case PriorityRole:
        return task.priority;
    case PriorityColorRole:
        return priorityToColor(task.priority);
    case PriorityIconRole:
        return priorityToIcon(task.priority);
    case DueDateRole:
        return task.dueDate;
    case CompletedRole:
        return task.completed;
    case TagsRole:
        return task.tags;
    case ProjectIdRole:
        return task.projectId;
    case PomodoroCountRole:
        return task.pomodoroCount;
    case EstimatedPomodorosRole:
        return task.estimatedPomodoros;
    case PomodoroProgressRole:
        if (task.estimatedPomodoros <= 0)
            return 0.0;
        return static_cast<double>(task.pomodoroCount) / task.estimatedPomodoros;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> TaskListModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    if (roles.isEmpty()) {
        roles[IdRole] = "taskId";
        roles[TitleRole] = "title";
        roles[DescriptionRole] = "description";
        roles[PriorityRole] = "priority";
        roles[PriorityColorRole] = "priorityColor";
        roles[PriorityIconRole] = "priorityIcon";
        roles[DueDateRole] = "dueDate";
        roles[CompletedRole] = "completed";
        roles[TagsRole] = "tags";
        roles[ProjectIdRole] = "projectId";
        roles[PomodoroCountRole] = "pomodoroCount";
        roles[EstimatedPomodorosRole] = "estimatedPomodoros";
        roles[PomodoroProgressRole] = "pomodoroProgress";
    }
    return roles;
}

int TaskListModel::count() const
{
    return static_cast<int>(m_tasks.size());
}

int TaskListModel::completedCount() const
{
    int completed = 0;
    for (const auto& task : m_tasks) {
        if (task.completed)
            ++completed;
    }
    return completed;
}

int TaskListModel::pendingCount() const
{
    return count() - completedCount();
}

void TaskListModel::setController(TaskController* controller)
{
    if (m_controller == controller)
        return;

    if (m_controller) {
        disconnect(m_controller, nullptr, this, nullptr);
    }

    m_controller = controller;

    if (m_controller) {
        connect(m_controller, &TaskController::taskCreated, this, &TaskListModel::refresh);
        connect(m_controller, &TaskController::taskUpdated, this, &TaskListModel::refresh);
        connect(m_controller, &TaskController::taskDeleted, this, &TaskListModel::refresh);
        connect(m_controller, &TaskController::taskCompleted, this, [this](int taskId, int xpGained) {
            refresh();
            emit taskCompleted(taskId, xpGained);
        });
    }
}

void TaskListModel::refresh()
{
    beginResetModel();
    loadTasks();
    endResetModel();
    
    emit countChanged();
    emit completedCountChanged();
    emit pendingCountChanged();
}

QVariantMap TaskListModel::getTask(int id) const
{
    QVariantMap result;
    for (const auto& task : m_tasks) {
        if (task.id == id) {
            result["id"] = task.id;
            result["title"] = task.title;
            result["description"] = task.description;
            result["priority"] = task.priority;
            result["priorityColor"] = priorityToColor(task.priority);
            result["dueDate"] = task.dueDate;
            result["completed"] = task.completed;
            result["tags"] = task.tags;
            result["pomodoroCount"] = task.pomodoroCount;
            result["estimatedPomodoros"] = task.estimatedPomodoros;
            break;
        }
    }
    return result;
}

void TaskListModel::setFilter(bool showCompleted, bool showPending)
{
    if (m_showCompleted == showCompleted && m_showPending == showPending)
        return;

    m_showCompleted = showCompleted;
    m_showPending = showPending;
    refresh();
}

void TaskListModel::loadTasks()
{
    m_tasks.clear();
    
    auto tasks = m_taskManager->getAllTasks();
    
    for (const auto& task : tasks) {
        // Apply filter
        if (!m_showCompleted && task.isCompleted())
            continue;
        if (!m_showPending && !task.isCompleted())
            continue;

        TaskItem item;
        item.id = task.getId();
        item.title = QString::fromStdString(task.getName());
        item.description = QString::fromStdString(task.getDescription());
        item.completed = task.isCompleted();
        item.projectId = task.getProjectId();
        // Note: Additional fields would be populated from the full Task entity
        // when the Task class includes all fields from entities.h
        item.priority = 1; // Default medium priority
        item.pomodoroCount = 0;
        item.estimatedPomodoros = 0;
        
        m_tasks.append(item);
    }
}

QString TaskListModel::priorityToColor(int priority) const
{
    switch (priority) {
    case 0: // Low
        return "#4CAF50"; // Green
    case 1: // Medium
        return "#FF9800"; // Orange
    case 2: // High
        return "#F44336"; // Red
    default:
        return "#9E9E9E"; // Grey
    }
}

QString TaskListModel::priorityToIcon(int priority) const
{
    switch (priority) {
    case 0:
        return "🟢"; // Green circle
    case 1:
        return "🟡"; // Yellow circle
    case 2:
        return "🔴"; // Red circle
    default:
        return "⚪"; // White circle
    }
}
