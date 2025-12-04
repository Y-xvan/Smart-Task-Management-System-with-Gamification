#include "Pomodoro/pomodoro.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <functional>

Pomodoro::Pomodoro(int work, int brk, int longBrk)
    : workDuration(work), breakDuration(brk), longBreakDuration(longBrk), 
      cycleCount(0), isRunning(false), shouldStop(false) {}

void Pomodoro::startWork() {
    std::cout << "Work session started for " << workDuration << " minutes." << std::endl;
    std::this_thread::sleep_for(std::chrono::minutes(workDuration));
    cycleCount++;
    std::cout << "Work session completed!" << std::endl;
}

void Pomodoro::startBreak() {
    std::cout << "Break started for " << breakDuration << " minutes." << std::endl;
    std::this_thread::sleep_for(std::chrono::minutes(breakDuration));
    std::cout << "Break finished!" << std::endl;
}

void Pomodoro::startLongBreak() {
    std::cout << "Long break started for " << longBreakDuration << " minutes." << std::endl;
    std::this_thread::sleep_for(std::chrono::minutes(longBreakDuration));
    std::cout << "Long break finished!" << std::endl;
}

bool Pomodoro::startWorkWithCountdown(std::function<void(int)> callback) {
    isRunning = true;
    shouldStop = false;
    int totalSeconds = workDuration * 60;
    
    for (int remaining = totalSeconds; remaining > 0 && !shouldStop; --remaining) {
        if (callback) {
            callback(remaining);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    if (!shouldStop) {
        cycleCount++;
    }
    isRunning = false;
    return !shouldStop;
}

bool Pomodoro::startBreakWithCountdown(std::function<void(int)> callback) {
    isRunning = true;
    shouldStop = false;
    int totalSeconds = breakDuration * 60;
    
    for (int remaining = totalSeconds; remaining > 0 && !shouldStop; --remaining) {
        if (callback) {
            callback(remaining);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    isRunning = false;
    return !shouldStop;
}

bool Pomodoro::startLongBreakWithCountdown(std::function<void(int)> callback) {
    isRunning = true;
    shouldStop = false;
    int totalSeconds = longBreakDuration * 60;
    
    for (int remaining = totalSeconds; remaining > 0 && !shouldStop; --remaining) {
        if (callback) {
            callback(remaining);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    isRunning = false;
    return !shouldStop;
}

void Pomodoro::stop() {
    shouldStop = true;
}

bool Pomodoro::getIsRunning() const {
    return isRunning;
}

int Pomodoro::getCycleCount() const {
    return cycleCount;
}

int Pomodoro::getWorkDuration() const {
    return workDuration;
}

int Pomodoro::getBreakDuration() const {
    return breakDuration;
}

int Pomodoro::getLongBreakDuration() const {
    return longBreakDuration;
}

void Pomodoro::setWorkDuration(int minutes) {
    if (minutes > 0 && minutes <= 120) {
        workDuration = minutes;
    }
}

void Pomodoro::setBreakDuration(int minutes) {
    if (minutes > 0 && minutes <= 60) {
        breakDuration = minutes;
    }
}

void Pomodoro::setLongBreakDuration(int minutes) {
    if (minutes > 0 && minutes <= 60) {
        longBreakDuration = minutes;
    }
}

void Pomodoro::resetCycleCount() {
    cycleCount = 0;
}
