#ifndef TASKMODEL_H
#define TASKMODEL_H

#include <QAbstractListModel>
#include <vector>
#include "task/TaskManager.h"
#include "project/ProjectManager.h"
#include "gamification/XPSystem.h"

class TaskModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum TaskRoles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        DescriptionRole,
        PriorityRole,
        XPRewardRole,
        CompletedRole,
        ProjectNameRole,
        DueDateRole,
        PomodoroCountRole,
        EstimatedPomodoroRole
    };

    TaskModel(TaskManager* tm, ProjectManager* pm, XPSystem* xp, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // QML 可调用接口
    Q_INVOKABLE void addTask(const QString& title, const QString& desc, int priority, int projectId, int estimatedPomo, const QString& dueDate);
    Q_INVOKABLE void completeTask(int index);
    Q_INVOKABLE void deleteTask(int index);
    Q_INVOKABLE void reload(); // 强制刷新

private:
    TaskManager* m_tm;
    ProjectManager* m_pm;
    XPSystem* m_xp;
    std::vector<Task> m_tasks;
};
#endif // TASKMODEL_H