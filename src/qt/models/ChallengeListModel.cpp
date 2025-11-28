/**
 * @file ChallengeListModel.cpp
 * @brief Implementation of ChallengeListModel
 */

#include "ChallengeListModel.h"

ChallengeListModel::ChallengeListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    loadChallenges();
}

ChallengeListModel::~ChallengeListModel() = default;

int ChallengeListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return static_cast<int>(m_challenges.size());
}

QVariant ChallengeListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_challenges.size())
        return QVariant();

    const ChallengeItem &challenge = m_challenges.at(index.row());

    switch (role) {
    case IdRole:
        return challenge.id;
    case TitleRole:
        return challenge.title;
    case DescriptionRole:
        return challenge.description;
    case TypeRole:
        return challenge.type;
    case TypeIconRole:
        return typeToIcon(challenge.type);
    case CriteriaRole:
        return challenge.criteria;
    case TargetValueRole:
        return challenge.targetValue;
    case CurrentValueRole:
        return challenge.currentValue;
    case ProgressRole:
        if (challenge.targetValue <= 0)
            return 0.0;
        return static_cast<double>(challenge.currentValue) / challenge.targetValue;
    case ProgressTextRole:
        return QString("%1/%2").arg(challenge.currentValue).arg(challenge.targetValue);
    case RewardXPRole:
        return challenge.rewardXP;
    case CompletedRole:
        return challenge.completed;
    case ClaimedRole:
        return challenge.claimed;
    case ExpiryDateRole:
        return challenge.expiryDate;
    case CategoryRole:
        return challenge.category;
    case CategoryIconRole:
        return categoryToIcon(challenge.category);
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> ChallengeListModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    if (roles.isEmpty()) {
        roles[IdRole] = "challengeId";
        roles[TitleRole] = "title";
        roles[DescriptionRole] = "description";
        roles[TypeRole] = "type";
        roles[TypeIconRole] = "typeIcon";
        roles[CriteriaRole] = "criteria";
        roles[TargetValueRole] = "targetValue";
        roles[CurrentValueRole] = "currentValue";
        roles[ProgressRole] = "progress";
        roles[ProgressTextRole] = "progressText";
        roles[RewardXPRole] = "rewardXP";
        roles[CompletedRole] = "completed";
        roles[ClaimedRole] = "claimed";
        roles[ExpiryDateRole] = "expiryDate";
        roles[CategoryRole] = "category";
        roles[CategoryIconRole] = "categoryIcon";
    }
    return roles;
}

int ChallengeListModel::count() const
{
    return static_cast<int>(m_challenges.size());
}

int ChallengeListModel::activeCount() const
{
    int active = 0;
    for (const auto& c : m_challenges) {
        if (!c.completed && !c.claimed)
            ++active;
    }
    return active;
}

int ChallengeListModel::completedCount() const
{
    int completed = 0;
    for (const auto& c : m_challenges) {
        if (c.completed)
            ++completed;
    }
    return completed;
}

void ChallengeListModel::refresh()
{
    beginResetModel();
    loadChallenges();
    endResetModel();
    
    emit countChanged();
    emit activeCountChanged();
    emit completedCountChanged();
}

void ChallengeListModel::filterByType(const QString& type)
{
    if (m_typeFilter == type)
        return;
    m_typeFilter = type;
    refresh();
}

void ChallengeListModel::showAll()
{
    m_typeFilter.clear();
    refresh();
}

void ChallengeListModel::loadChallenges()
{
    m_challenges.clear();
    
    // TODO: Replace with actual database query when ChallengeDAO is implemented
    // For now, load sample challenges for UI demonstration
    loadSampleChallenges();
    
    // Apply filter if set
    if (!m_typeFilter.isEmpty()) {
        QList<ChallengeItem> filtered;
        for (const auto& c : m_challenges) {
            if (c.type == m_typeFilter)
                filtered.append(c);
        }
        m_challenges = filtered;
    }
}

void ChallengeListModel::loadSampleChallenges()
{
    // Daily challenges
    {
        ChallengeItem c;
        c.id = 1;
        c.title = QString::fromUtf8("任务新手");
        c.description = QString::fromUtf8("今天完成3个任务");
        c.type = "daily";
        c.targetValue = 3;
        c.currentValue = 1;
        c.rewardXP = 50;
        c.completed = false;
        c.category = "task";
        m_challenges.append(c);
    }
    
    {
        ChallengeItem c;
        c.id = 2;
        c.title = QString::fromUtf8("番茄钟达人");
        c.description = QString::fromUtf8("完成5个番茄钟");
        c.type = "daily";
        c.targetValue = 5;
        c.currentValue = 2;
        c.rewardXP = 75;
        c.completed = false;
        c.category = "pomodoro";
        m_challenges.append(c);
    }

    // Weekly challenges
    {
        ChallengeItem c;
        c.id = 3;
        c.title = QString::fromUtf8("周任务大师");
        c.description = QString::fromUtf8("本周完成15个任务");
        c.type = "weekly";
        c.targetValue = 15;
        c.currentValue = 7;
        c.rewardXP = 200;
        c.completed = false;
        c.category = "task";
        m_challenges.append(c);
    }
    
    {
        ChallengeItem c;
        c.id = 4;
        c.title = QString::fromUtf8("项目推进者");
        c.description = QString::fromUtf8("使项目进度达到50%");
        c.type = "weekly";
        c.targetValue = 50;
        c.currentValue = 35;
        c.rewardXP = 150;
        c.completed = false;
        c.category = "project";
        m_challenges.append(c);
    }

    // Monthly challenge
    {
        ChallengeItem c;
        c.id = 5;
        c.title = QString::fromUtf8("月度冠军");
        c.description = QString::fromUtf8("本月完成50个任务");
        c.type = "monthly";
        c.targetValue = 50;
        c.currentValue = 23;
        c.rewardXP = 500;
        c.completed = false;
        c.category = "task";
        m_challenges.append(c);
    }
}

QString ChallengeListModel::typeToIcon(const QString& type) const
{
    if (type == "daily")
        return QString::fromUtf8("📅");
    else if (type == "weekly")
        return QString::fromUtf8("📆");
    else if (type == "monthly")
        return QString::fromUtf8("🗓️");
    return QString::fromUtf8("🎯");
}

QString ChallengeListModel::categoryToIcon(const QString& category) const
{
    if (category == "task")
        return QString::fromUtf8("📋");
    else if (category == "pomodoro")
        return QString::fromUtf8("🍅");
    else if (category == "project")
        return QString::fromUtf8("📁");
    return QString::fromUtf8("⚡");
}
