/**
 * @file ProjectListModel.h
 * @brief QAbstractListModel for Project data - bridges C++ ProjectManager to QML
 */

#ifndef PROJECTLISTMODEL_H
#define PROJECTLISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <memory>

class ProjectManager;

/**
 * @brief Project data structure for QML binding
 */
struct ProjectItem {
    int id = 0;
    QString name;
    QString description;
    QString colorLabel;
    double progress = 0.0;
    int totalTasks = 0;
    int completedTasks = 0;
    QString targetDate;
    bool archived = false;
    QString createdDate;
};

/**
 * @brief QAbstractListModel implementation for Project list
 */
class ProjectListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum ProjectRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        DescriptionRole,
        ColorLabelRole,
        ProgressRole,
        TotalTasksRole,
        CompletedTasksRole,
        TargetDateRole,
        ArchivedRole,
        CreatedDateRole,
        ProgressTextRole
    };

    explicit ProjectListModel(QObject *parent = nullptr);
    ~ProjectListModel() override;

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;

public slots:
    void refresh();
    Q_INVOKABLE QVariantMap getProject(int id) const;
    Q_INVOKABLE void setShowArchived(bool show);

signals:
    void countChanged();
    void projectSelected(int projectId);

private:
    QList<ProjectItem> m_projects;
    std::unique_ptr<ProjectManager> m_projectManager;
    bool m_showArchived = false;

    void loadProjects();
};

#endif // PROJECTLISTMODEL_H
