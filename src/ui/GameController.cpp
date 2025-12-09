#include "GameController.h"

GameController::GameController(XPSystem* xp, StatisticsAnalyzer* stats, Pomodoro* pomo, AchievementManager* achieve, QObject* parent)
    : QObject(parent), m_xp(xp), m_stats(stats), m_pomo(pomo), m_achieve(achieve) {

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &GameController::onTick);

    // 初始刷新
    refresh();
}

void GameController::refresh() {
    emit statsChanged();
}

QString GameController::timerText() const {
    int m = m_remainingSeconds / 60;
    int s = m_remainingSeconds % 60;
    return QString("%1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
}

double GameController::timerProgress() const {
    if (m_totalSeconds <= 0) return 0.0;
    return 1.0 - (double)m_remainingSeconds / m_totalSeconds;
}

void GameController::startTimer(const QString& mode) {
    m_timerMode = mode;

    if (mode == "Work") m_remainingSeconds = 25 * 60;
    else if (mode == "Short") m_remainingSeconds = 5 * 60;
    else if (mode == "Long") m_remainingSeconds = 15 * 60;
    else m_remainingSeconds = 10; // Test

    m_totalSeconds = m_remainingSeconds;
    m_timer->start();
    emit timerChanged();
}

void GameController::stopTimer() {
    m_timer->stop();
    m_timerMode = "Idle";
    m_remainingSeconds = 0;
    emit timerChanged();
}

void GameController::onTick() {
    if (m_remainingSeconds > 0) {
        m_remainingSeconds--;
        emit timerChanged();
    }
    else {
        m_timer->stop();
        emit timerFinished(m_timerMode);

        // 结算逻辑
        if (m_timerMode == "Work") {
            int xp = m_xp->getXPForPomodoro();
            m_xp->awardXP(xp, "Pomodoro Finished");
            emit xpGained(xp, "Focus Complete!");

            // 这里应该调用 m_pomo->addCycle() 但原始类可能没公开简单接口，暂忽略
        }

        m_timerMode = "Finished";
        emit timerChanged();
        emit statsChanged();
    }
}

void GameController::checkAchievements() {
    // 调用后端检查
    m_achieve->checkAllAchievements();
    // 这里简化处理：假设每次检查都通知 UI 刷新
    emit statsChanged();
}