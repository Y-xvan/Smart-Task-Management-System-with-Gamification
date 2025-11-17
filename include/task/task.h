#ifndef TASK_H
#define TASK_H

#include <string>

class Task {
private:
    std::string name;
    std::string description;
    bool completed;

public:
    Task(const std::string &name, const std::string &desc);

    void markCompleted();
    bool isCompleted() const;

    std::string getName() const;
    std::string getDescription() const;
};

#endif // TASK_H
