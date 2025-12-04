#ifndef TASKMODEL_H
#define TASKMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <QList>
#include <memory>

#include "task/TaskManager.h"
#include "gamification/XPSystem.h"

/**
 * @brief TaskModel - Qt model adapter for exposing tasks to QML
 * 
 * Inherits from QAbstractListModel to provide task data to QML ListView.
 * Styled as an "RPG Quest Board" interface.
 */
class TaskModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int level READ level NOTIFY userStatsChanged)
    Q_PROPERTY(int totalXP READ totalXP NOTIFY userStatsChanged)
    Q_PROPERTY(QString levelTitle READ levelTitle NOTIFY userStatsChanged)
    Q_PROPERTY(double levelProgress READ levelProgress NOTIFY userStatsChanged)

public:
    /**
     * @brief Role names for QML data binding
     */
    enum TaskRoles {
        TitleRole = Qt::UserRole + 1,
        DescriptionRole,
        PriorityRole,
        DueDateRole,
        XPRewardRole,
        IsCompletedRole,
        TaskIdRole
    };
    Q_ENUM(TaskRoles)

    explicit TaskModel(QObject *parent = nullptr);
    ~TaskModel() override;

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // User stats properties
    int level() const;
    int totalXP() const;
    QString levelTitle() const;
    double levelProgress() const;

public slots:
    /**
     * @brief Mark a task as completed and award XP
     * @param index The row index of the task in the model
     * @return true if successful
     */
    bool markTaskCompleted(int index);

    /**
     * @brief Reload tasks from the database
     */
    void refreshTasks();

signals:
    void userStatsChanged();
    void taskCompleted(int xpAwarded, QString taskTitle);

private:
    /**
     * @brief Internal task representation for QML
     */
    struct TaskData {
        int id;
        QString title;
        QString description;
        int priority;
        QString dueDate;
        int xpReward;
        bool isCompleted;
    };

    QList<TaskData> m_tasks;
    std::unique_ptr<TaskManager> m_taskManager;
    std::unique_ptr<XPSystem> m_xpSystem;

    /**
     * @brief Calculate XP reward based on task priority
     * @param priority Task priority (0=low, 1=medium, 2=high)
     * @return XP reward amount
     */
    int calculateXPReward(int priority) const;

    /**
     * @brief Load tasks from backend into model
     */
    void loadTasks();
};

#endif // TASKMODEL_H
