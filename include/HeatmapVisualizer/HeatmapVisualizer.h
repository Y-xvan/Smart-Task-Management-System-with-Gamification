#ifndef HEATMAP_VISUALIZER_H
#define HEATMAP_VISUALIZER_H

#include <string>
#include <map>
#include <vector>
using namespace std;

class HeatmapVisualizer {
private:
    // 模拟数据：日期 -> 任务完成数
    map<string, int> taskData;
    
    // 根据任务数量获取颜色字符
    string getColorBlock(int count);
    
    // 获取某个日期的任务数
    int getTaskCount(string date);
    
    // 生成日期列表
    vector<string> generateDateRange(int days);
    
public:
    HeatmapVisualizer();
    
    // 添加任务数据（模拟）
    void addTaskData(string date, int count);
    
    // 生成90天热力图
    string generateHeatmap(int days = 90);
    
    // 生成月视图
    string generateMonthView(string month);
    
    // 生成周视图
    string generateWeekView(string startDate);
    
    // 获取统计信息
    int getTotalTasks();
    string getMostActiveDay();
    int getCurrentStreak();
};

#endif