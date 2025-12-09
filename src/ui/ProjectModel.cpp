#include "ProjectModel.h"

ProjectModel::ProjectModel(ProjectManager* pm, QObject* parent)
    : QAbstractListModel(parent), m_pm(pm) {
    reload();
}

int ProjectModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_projects.size());
}

QVariant ProjectModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_projects.size())
        return QVariant();

    Project* p = m_projects[index.row()];

    switch (role) {
    case IdRole: return p->getId();
    case NameRole: return QString::fromStdString(p->getName());
    case DescRole: return QString::fromStdString(p->getDescription());
    case ColorRole: return QString::fromStdString(p->getColorLabel());
    case ProgressRole: return p->getProgress(); // 0.0 - 1.0
    case TaskCountRole: return QString("%1/%2").arg(p->getCompletedTasks()).arg(p->getTotalTasks());
    }
    return QVariant();
}

QHash<int, QByteArray> ProjectModel::roleNames() const {
    return {
        {IdRole, "projectId"},
        {NameRole, "name"},
        {DescRole, "description"},
        {ColorRole, "colorCode"},
        {ProgressRole, "progress"},
        {TaskCountRole, "taskCountText"}
    };
}

void ProjectModel::addProject(const QString& name, const QString& desc, const QString& color) {
    Project p(name.toStdString(), desc.toStdString(), color.toStdString());
    m_pm->createProject(p);
    reload();
}

void ProjectModel::deleteProject(int index) {
    if (index < 0 || index >= m_projects.size()) return;
    m_pm->deleteProject(m_projects[index]->getId());
    reload();
}

void ProjectModel::reload() {
    beginResetModel();
    m_projects = m_pm->getAllProjects();
    endResetModel();
}

QStringList ProjectModel::getProjectNames() {
    QStringList names;
    for (auto* p : m_projects) {
        names.append(QString::fromStdString(p->getName()));
    }
    return names;
}

int ProjectModel::getProjectId(int index) {
    if (index < 0 || index >= m_projects.size()) return -1;
    return m_projects[index]->getId();
}