/**
 * @file ProjectListModel.cpp
 * @brief Implementation of ProjectListModel
 */

#include "ProjectListModel.h"
#include "project/ProjectManager.h"
#include "project/Project.h"

ProjectListModel::ProjectListModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_projectManager(std::make_unique<ProjectManager>())
{
    loadProjects();
}

ProjectListModel::~ProjectListModel() = default;

int ProjectListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_projects.size());
}

QVariant ProjectListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_projects.size())
        return QVariant();

    const ProjectItem &project = m_projects.at(index.row());

    switch (role) {
    case IdRole:
        return project.id;
    case NameRole:
        return project.name;
    case DescriptionRole:
        return project.description;
    case ColorLabelRole:
        return project.colorLabel;
    case ProgressRole:
        return project.progress;
    case TotalTasksRole:
        return project.totalTasks;
    case CompletedTasksRole:
        return project.completedTasks;
    case TargetDateRole:
        return project.targetDate;
    case ArchivedRole:
        return project.archived;
    case CreatedDateRole:
        return project.createdDate;
    case ProgressTextRole:
        return QString("%1/%2").arg(project.completedTasks).arg(project.totalTasks);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ProjectListModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    if (roles.isEmpty()) {
        roles[IdRole] = "projectId";
        roles[NameRole] = "name";
        roles[DescriptionRole] = "description";
        roles[ColorLabelRole] = "colorLabel";
        roles[ProgressRole] = "progress";
        roles[TotalTasksRole] = "totalTasks";
        roles[CompletedTasksRole] = "completedTasks";
        roles[TargetDateRole] = "targetDate";
        roles[ArchivedRole] = "archived";
        roles[CreatedDateRole] = "createdDate";
        roles[ProgressTextRole] = "progressText";
    }
    return roles;
}

int ProjectListModel::count() const
{
    return static_cast<int>(m_projects.size());
}

void ProjectListModel::refresh()
{
    beginResetModel();
    loadProjects();
    endResetModel();
    emit countChanged();
}

QVariantMap ProjectListModel::getProject(int id) const
{
    QVariantMap result;
    for (const auto& project : m_projects) {
        if (project.id == id) {
            result["id"] = project.id;
            result["name"] = project.name;
            result["description"] = project.description;
            result["colorLabel"] = project.colorLabel;
            result["progress"] = project.progress;
            result["totalTasks"] = project.totalTasks;
            result["completedTasks"] = project.completedTasks;
            result["targetDate"] = project.targetDate;
            result["archived"] = project.archived;
            break;
        }
    }
    return result;
}

void ProjectListModel::setShowArchived(bool show)
{
    if (m_showArchived == show)
        return;
    m_showArchived = show;
    refresh();
}

void ProjectListModel::loadProjects()
{
    m_projects.clear();
    
    auto projects = m_projectManager->getAllProjects();
    
    for (Project* p : projects) {
        if (!m_showArchived && p->isArchived())
            continue;

        ProjectItem item;
        item.id = p->getId();
        item.name = QString::fromStdString(p->getName());
        item.description = QString::fromStdString(p->getDescription());
        item.colorLabel = QString::fromStdString(p->getColorLabel());
        item.progress = p->getProgress();
        item.totalTasks = p->getTotalTasks();
        item.completedTasks = p->getCompletedTasks();
        item.targetDate = QString::fromStdString(p->getTargetDate());
        item.archived = p->isArchived();
        item.createdDate = QString::fromStdString(p->getCreatedDate());
        
        m_projects.append(item);
    }
}
