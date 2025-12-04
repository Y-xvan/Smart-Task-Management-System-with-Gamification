#include "ui/TaskModel.h"
#include "database/DatabaseManager.h"

TaskModel::TaskModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_taskManager(std::make_unique<TaskManager>())
    , m_xpSystem(std::make_unique<XPSystem>())
{
    // Initialize database
    DatabaseManager& db = DatabaseManager::getInstance();
    if (!db.isOpen()) {
        db.initialize("task_manager.db");
    }
    
    // Initialize task manager
    m_taskManager->initialize();
    
    // Load initial tasks
    loadTasks();
}

TaskModel::~TaskModel() = default;

int TaskModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_tasks.count();
}

QVariant TaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_tasks.count())
        return QVariant();

    const TaskData &task = m_tasks.at(index.row());

    switch (role) {
    case TitleRole:
        return task.title;
    case DescriptionRole:
        return task.description;
    case PriorityRole:
        return task.priority;
    case DueDateRole:
        return task.dueDate;
    case XPRewardRole:
        return task.xpReward;
    case IsCompletedRole:
        return task.isCompleted;
    case TaskIdRole:
        return task.id;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> TaskModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[DescriptionRole] = "description";
    roles[PriorityRole] = "priority";
    roles[DueDateRole] = "dueDate";
    roles[XPRewardRole] = "xpReward";
    roles[IsCompletedRole] = "isCompleted";
    roles[TaskIdRole] = "taskId";
    return roles;
}

int TaskModel::level() const
{
    return m_xpSystem->getCurrentLevel();
}

int TaskModel::totalXP() const
{
    return m_xpSystem->getTotalXP();
}

QString TaskModel::levelTitle() const
{
    return QString::fromStdString(m_xpSystem->getCurrentLevelTitle());
}

double TaskModel::levelProgress() const
{
    return m_xpSystem->getLevelProgress();
}

bool TaskModel::markTaskCompleted(int index)
{
    if (index < 0 || index >= m_tasks.count())
        return false;

    TaskData &task = m_tasks[index];
    
    if (task.isCompleted)
        return false; // Already completed

    // Mark task as completed in backend
    if (!m_taskManager->completeTask(task.id))
        return false;

    // Award XP based on priority
    int xpAmount = calculateXPReward(task.priority);
    std::string source = "Quest completed: " + task.title.toStdString();
    m_xpSystem->awardXP(xpAmount, source);

    // Update local model
    task.isCompleted = true;

    // Notify QML of changes
    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex, {IsCompletedRole});
    emit userStatsChanged();
    emit taskCompleted(xpAmount, task.title);

    return true;
}

void TaskModel::refreshTasks()
{
    beginResetModel();
    loadTasks();
    endResetModel();
    emit userStatsChanged();
}

int TaskModel::calculateXPReward(int priority) const
{
    return m_xpSystem->getXPForTaskCompletion(priority);
}

void TaskModel::loadTasks()
{
    m_tasks.clear();
    
    std::vector<Task> allTasks = m_taskManager->getAllTasks();
    
    for (const Task &task : allTasks) {
        TaskData data;
        data.id = task.getId();
        data.title = QString::fromStdString(task.getName());
        data.description = QString::fromStdString(task.getDescription());
        data.priority = task.getPriority();
        data.dueDate = QString::fromStdString(task.getDueDate());
        data.xpReward = calculateXPReward(task.getPriority());
        data.isCompleted = task.isCompleted();
        m_tasks.append(data);
    }
}
