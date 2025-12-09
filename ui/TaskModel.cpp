#include "TaskModel.h"
#include <QDebug>

TaskModel::TaskModel(TaskManager* tm, ProjectManager* pm, XPSystem* xp, QObject* parent)
    : QAbstractListModel(parent), m_tm(tm), m_pm(pm), m_xp(xp) {
    reload();
}

int TaskModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_tasks.size());
}

QVariant TaskModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_tasks.size())
        return QVariant();

    const Task& t = m_tasks[index.row()];

    switch (role) {
    case IdRole: return t.getId();
    case TitleRole: return QString::fromStdString(t.getName());
    case DescriptionRole: return QString::fromStdString(t.getDescription());
    case PriorityRole: return t.getPriority();
    case XPRewardRole: return m_xp->getXPForTaskCompletion(t.getPriority());
    case CompletedRole: return t.isCompleted();
    case DueDateRole: return QString::fromStdString(t.getDueDate());
    case PomodoroCountRole: return t.getPomodoroCount();
    case EstimatedPomodoroRole: return t.getEstimatedPomodoros();
    case ProjectNameRole: {
        auto pid = t.getProjectId();
        if (pid.has_value()) {
            auto p = m_pm->getProject(pid.value());
            if (p) return QString::fromStdString(p->getName());
        }
        return QString("");
    }
    }
    return QVariant();
}

QHash<int, QByteArray> TaskModel::roleNames() const {
    return {
        {IdRole, "taskId"},
        {TitleRole, "title"},
        {DescriptionRole, "description"},
        {PriorityRole, "priority"},
        {XPRewardRole, "xpReward"},
        {CompletedRole, "completed"},
        {ProjectNameRole, "projectName"},
        {DueDateRole, "dueDate"},
        {PomodoroCountRole, "pomodoroCount"},
        {EstimatedPomodoroRole, "estimatedPomodoros"}
    };
}

void TaskModel::addTask(const QString& title, const QString& desc, int priority, int projectId, int estimatedPomo, const QString& dueDate) {
    Task t(title.toStdString(), desc.toStdString());
    t.setPriority(priority);
    t.setEstimatedPomodoros(estimatedPomo);
    if (!dueDate.isEmpty()) t.setDueDate(dueDate.toStdString());
    if (projectId > 0) t.setProjectId(projectId);

    m_tm->createTask(t);
    reload();
}

void TaskModel::completeTask(int index) {
    if (index < 0 || index >= m_tasks.size()) return;
    int id = m_tasks[index].getId();

    if (m_tm->completeTask(id)) {
        // 自动奖励 XP
        int prio = m_tasks[index].getPriority();
        int xp = m_xp->getXPForTaskCompletion(prio);
        m_xp->awardXP(xp, "Completed Task: " + m_tasks[index].getName());
        reload();
    }
}

void TaskModel::deleteTask(int index) {
    if (index < 0 || index >= m_tasks.size()) return;
    int id = m_tasks[index].getId();
    m_tm->deleteTask(id);
    reload();
}

void TaskModel::reload() {
    beginResetModel();
    std::vector<Task> all = m_tm->getAllTasks();
    m_tasks.clear();
    // 仅展示未完成任务，符合 Quest Board 逻辑
    for (const auto& t : all) {
        if (!t.isCompleted()) {
            m_tasks.push_back(t);
        }
    }
    endResetModel();
}