#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include <QAbstractListModel>
#include <vector>
#include "project/ProjectManager.h"

class ProjectModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum ProjectRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        DescRole,
        ColorRole,
        ProgressRole,
        TaskCountRole
    };

    ProjectModel(ProjectManager* pm, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addProject(const QString& name, const QString& desc, const QString& color);
    Q_INVOKABLE void deleteProject(int index);
    Q_INVOKABLE void reload();
    Q_INVOKABLE QStringList getProjectNames(); // 供 ComboBox 使用
    Q_INVOKABLE int getProjectId(int index); // 供 ComboBox 映射 ID

private:
    ProjectManager* m_pm;
    std::vector<Project*> m_projects;
};
#endif // PROJECTMODEL_H