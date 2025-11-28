/**
 * @file ChallengeListModel.h
 * @brief QAbstractListModel for Challenge data - gamification feature
 */

#ifndef CHALLENGELISTMODEL_H
#define CHALLENGELISTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "entities.h"

/**
 * @brief Challenge data structure for QML binding
 */
struct ChallengeItem {
    int id = 0;
    QString title;
    QString description;
    QString type;        // "daily", "weekly", "monthly"
    QString criteria;
    int targetValue = 0;
    int currentValue = 0;
    int rewardXP = 0;
    bool completed = false;
    bool claimed = false;
    QString expiryDate;
    QString category;    // "task", "pomodoro", "project"
};

/**
 * @brief QAbstractListModel implementation for Challenge list
 */
class ChallengeListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int activeCount READ activeCount NOTIFY activeCountChanged)
    Q_PROPERTY(int completedCount READ completedCount NOTIFY completedCountChanged)

public:
    enum ChallengeRoles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        DescriptionRole,
        TypeRole,
        TypeIconRole,
        CriteriaRole,
        TargetValueRole,
        CurrentValueRole,
        ProgressRole,
        ProgressTextRole,
        RewardXPRole,
        CompletedRole,
        ClaimedRole,
        ExpiryDateRole,
        CategoryRole,
        CategoryIconRole
    };

    explicit ChallengeListModel(QObject *parent = nullptr);
    ~ChallengeListModel() override;

    // QAbstractListModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    int count() const;
    int activeCount() const;
    int completedCount() const;

public slots:
    void refresh();
    Q_INVOKABLE void filterByType(const QString& type);
    Q_INVOKABLE void showAll();

signals:
    void countChanged();
    void activeCountChanged();
    void completedCountChanged();
    void challengeCompleted(int challengeId, int xpReward);

private:
    QList<ChallengeItem> m_challenges;
    QString m_typeFilter;

    void loadChallenges();
    QString typeToIcon(const QString& type) const;
    QString categoryToIcon(const QString& category) const;
    
    // Sample data for demonstration
    void loadSampleChallenges();
};

#endif // CHALLENGELISTMODEL_H
