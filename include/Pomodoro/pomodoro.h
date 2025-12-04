#ifndef POMODORO_H
#define POMODORO_H

#include <functional>

class Pomodoro {
private:
    int workDuration;       // minutes
    int breakDuration;      // minutes
    int longBreakDuration;  // minutes
    int cycleCount;
    bool isRunning;
    bool shouldStop;

public:
    Pomodoro(int work = 25, int brk = 5, int longBrk = 15);

    // 启动计时器（阻塞版本）
    void startWork();
    void startBreak();
    void startLongBreak();

    // 启动带回调的计时器（可显示倒计时）
    // callback: 每秒回调一次，参数为剩余秒数
    // 返回值: true表示正常完成，false表示被中断
    bool startWorkWithCountdown(std::function<void(int)> callback);
    bool startBreakWithCountdown(std::function<void(int)> callback);
    bool startLongBreakWithCountdown(std::function<void(int)> callback);

    // 停止当前计时
    void stop();
    bool getIsRunning() const;

    // 获取器
    int getCycleCount() const;
    int getWorkDuration() const;
    int getBreakDuration() const;
    int getLongBreakDuration() const;

    // 设置器
    void setWorkDuration(int minutes);
    void setBreakDuration(int minutes);
    void setLongBreakDuration(int minutes);
    void resetCycleCount();
};

#endif // POMODORO_H
