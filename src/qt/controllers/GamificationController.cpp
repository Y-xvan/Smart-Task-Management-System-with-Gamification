/**
 * @file GamificationController.cpp
 * @brief Implementation of GamificationController
 */

#include "GamificationController.h"
#include "gamification/XPSystem.h"
#include "statistics/StatisticsAnalyzer.h"

GamificationController::GamificationController(QObject *parent)
    : QObject(parent)
    , m_xpSystem(std::make_unique<XPSystem>())
    , m_statsAnalyzer(std::make_unique<StatisticsAnalyzer>())
{
    m_previousLevel = currentLevel();
}

GamificationController::~GamificationController() = default;

int GamificationController::currentLevel() const
{
    return m_xpSystem->getCurrentLevel();
}

int GamificationController::currentXP() const
{
    return m_xpSystem->getCurrentXP();
}

int GamificationController::totalXP() const
{
    return m_xpSystem->getTotalXP();
}

int GamificationController::xpForNextLevel() const
{
    return m_xpSystem->getXPForNextLevel();
}

double GamificationController::levelProgress() const
{
    return m_xpSystem->getLevelProgress();
}

QString GamificationController::levelTitle() const
{
    return QString::fromStdString(m_xpSystem->getCurrentLevelTitle());
}

QString GamificationController::levelBadge() const
{
    return QString::fromStdString(m_xpSystem->getLevelBadge(currentLevel()));
}

int GamificationController::currentStreak() const
{
    return m_statsAnalyzer->getCurrentStreak();
}

int GamificationController::longestStreak() const
{
    return m_statsAnalyzer->getLongestStreak();
}

int GamificationController::achievementsUnlocked() const
{
    return m_statsAnalyzer->getAchievementsUnlocked();
}

int GamificationController::challengesCompleted() const
{
    return m_statsAnalyzer->getChallengesCompleted();
}

int GamificationController::tasksCompletedToday() const
{
    return m_statsAnalyzer->getTasksCompletedToday();
}

int GamificationController::tasksCompletedThisWeek() const
{
    return m_statsAnalyzer->getTasksCompletedThisWeek();
}

double GamificationController::completionRate() const
{
    return m_statsAnalyzer->getCompletionRate();
}

void GamificationController::refresh()
{
    emit levelChanged();
    emit xpChanged();
    emit streakChanged();
    emit achievementsChanged();
    emit challengesChanged();
    emit statsChanged();
    
    checkLevelUp();
}

QString GamificationController::getXPInfoDisplay() const
{
    return QString::fromStdString(m_xpSystem->displayLevelInfo());
}

QString GamificationController::getXPBarDisplay(int width) const
{
    return QString::fromStdString(m_xpSystem->displayXPBar(width));
}

int GamificationController::getXPForTask(int priority) const
{
    return m_xpSystem->getXPForTaskCompletion(priority);
}

int GamificationController::getXPForPomodoro() const
{
    return m_xpSystem->getXPForPomodoro();
}

void GamificationController::checkLevelUp()
{
    int newLevel = currentLevel();
    if (newLevel > m_previousLevel) {
        emit levelUp(newLevel, levelTitle());
        m_previousLevel = newLevel;
    }
}
