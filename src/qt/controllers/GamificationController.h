/**
 * @file GamificationController.h
 * @brief Controller for gamification features - XP, levels, achievements, challenges
 */

#ifndef GAMIFICATIONCONTROLLER_H
#define GAMIFICATIONCONTROLLER_H

#include <QObject>
#include <QString>
#include <memory>

class XPSystem;
class StatisticsAnalyzer;

/**
 * @brief Controller for gamification-related operations
 * 
 * Exposes gamification data to QML for the HUD and gamification pages.
 */
class GamificationController : public QObject
{
    Q_OBJECT
    
    // Level and XP properties
    Q_PROPERTY(int currentLevel READ currentLevel NOTIFY levelChanged)
    Q_PROPERTY(int currentXP READ currentXP NOTIFY xpChanged)
    Q_PROPERTY(int totalXP READ totalXP NOTIFY xpChanged)
    Q_PROPERTY(int xpForNextLevel READ xpForNextLevel NOTIFY levelChanged)
    Q_PROPERTY(double levelProgress READ levelProgress NOTIFY xpChanged)
    Q_PROPERTY(QString levelTitle READ levelTitle NOTIFY levelChanged)
    Q_PROPERTY(QString levelBadge READ levelBadge NOTIFY levelChanged)
    
    // Statistics properties
    Q_PROPERTY(int currentStreak READ currentStreak NOTIFY streakChanged)
    Q_PROPERTY(int longestStreak READ longestStreak NOTIFY streakChanged)
    Q_PROPERTY(int achievementsUnlocked READ achievementsUnlocked NOTIFY achievementsChanged)
    Q_PROPERTY(int challengesCompleted READ challengesCompleted NOTIFY challengesChanged)
    Q_PROPERTY(int tasksCompletedToday READ tasksCompletedToday NOTIFY statsChanged)
    Q_PROPERTY(int tasksCompletedThisWeek READ tasksCompletedThisWeek NOTIFY statsChanged)
    Q_PROPERTY(double completionRate READ completionRate NOTIFY statsChanged)

public:
    explicit GamificationController(QObject *parent = nullptr);
    ~GamificationController() override;

    // Level and XP accessors
    int currentLevel() const;
    int currentXP() const;
    int totalXP() const;
    int xpForNextLevel() const;
    double levelProgress() const;
    QString levelTitle() const;
    QString levelBadge() const;

    // Statistics accessors
    int currentStreak() const;
    int longestStreak() const;
    int achievementsUnlocked() const;
    int challengesCompleted() const;
    int tasksCompletedToday() const;
    int tasksCompletedThisWeek() const;
    double completionRate() const;

    /**
     * @brief Refresh all gamification data
     */
    Q_INVOKABLE void refresh();

    /**
     * @brief Get XP info display string
     */
    Q_INVOKABLE QString getXPInfoDisplay() const;

    /**
     * @brief Get XP progress bar string (ASCII art)
     */
    Q_INVOKABLE QString getXPBarDisplay(int width = 20) const;

    /**
     * @brief Get reward XP for a task of given priority
     */
    Q_INVOKABLE int getXPForTask(int priority) const;

    /**
     * @brief Get reward XP for a pomodoro
     */
    Q_INVOKABLE int getXPForPomodoro() const;

signals:
    void levelChanged();
    void xpChanged();
    void streakChanged();
    void achievementsChanged();
    void challengesChanged();
    void statsChanged();
    void levelUp(int newLevel, const QString& newTitle);

private:
    std::unique_ptr<XPSystem> m_xpSystem;
    std::unique_ptr<StatisticsAnalyzer> m_statsAnalyzer;

    void checkLevelUp();
    int m_previousLevel = 0;
};

#endif // GAMIFICATIONCONTROLLER_H
