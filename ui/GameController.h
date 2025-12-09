#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QString>
#include <vector>

#include "gamification/XPSystem.h"
#include "statistics/StatisticsAnalyzer.h"
#include "Pomodoro/pomodoro.h"
#include "achievement/AchievementManager.h"

class GameController : public QObject {
    Q_OBJECT

        // === 游戏化属性 (HUD) ===
        Q_PROPERTY(int currentLevel READ currentLevel NOTIFY statsChanged)
        Q_PROPERTY(int currentXP READ currentXP NOTIFY statsChanged)
        Q_PROPERTY(int nextLevelXP READ nextLevelXP NOTIFY statsChanged)
        Q_PROPERTY(double levelProgress READ levelProgress NOTIFY statsChanged)
        Q_PROPERTY(QString currentTitle READ currentTitle NOTIFY statsChanged)
        Q_PROPERTY(int streakDays READ streakDays NOTIFY statsChanged)

        // === 番茄钟属性 ===
        Q_PROPERTY(QString timerText READ timerText NOTIFY timerChanged)
        Q_PROPERTY(double timerProgress READ timerProgress NOTIFY timerChanged)
        Q_PROPERTY(bool isTimerRunning READ isTimerRunning NOTIFY timerChanged)
        Q_PROPERTY(QString timerMode READ timerMode NOTIFY timerChanged)

        // === 统计属性 ===
        Q_PROPERTY(int totalTasks READ totalTasks NOTIFY statsChanged)
        Q_PROPERTY(int totalPomodoros READ totalPomodoros NOTIFY statsChanged)

public:
    GameController(XPSystem* xp, StatisticsAnalyzer* stats, Pomodoro* pomo, AchievementManager* achieve, QObject* parent = nullptr);

    // Getters
    int currentLevel() const { return m_xp->getCurrentLevel(); }
    int currentXP() const { return m_xp->getCurrentXP(); }
    int nextLevelXP() const { return m_xp->getXPForNextLevel(); }
    double levelProgress() const { return m_xp->getLevelProgress(); }
    QString currentTitle() const { return QString::fromStdString(m_xp->getCurrentLevelTitle()); }
    int streakDays() const { return m_stats->getCurrentStreak(); }

    int totalTasks() const { return m_stats->getTotalTasksCompleted(); }
    int totalPomodoros() const { return m_stats->getTotalPomodoros(); }

    // Timer Getters
    QString timerText() const;
    double timerProgress() const;
    bool isTimerRunning() const { return m_timer->isActive(); }
    QString timerMode() const { return m_timerMode; }

    // Invokables
    Q_INVOKABLE void startTimer(const QString& mode); // "Work", "Short", "Long"
    Q_INVOKABLE void stopTimer();
    Q_INVOKABLE void checkAchievements();
    Q_INVOKABLE void refresh();

signals:
    void statsChanged();
    void timerChanged();
    void timerFinished(QString mode);
    void xpGained(int amount, QString reason);
    void achievementUnlocked(QString name, QString desc);

private slots:
    void onTick();

private:
    XPSystem* m_xp;
    StatisticsAnalyzer* m_stats;
    Pomodoro* m_pomo;
    AchievementManager* m_achieve;

    QTimer* m_timer;
    int m_remainingSeconds = 0;
    int m_totalSeconds = 1;
    QString m_timerMode = "Idle";
};
#endif // GAMECONTROLLER_H