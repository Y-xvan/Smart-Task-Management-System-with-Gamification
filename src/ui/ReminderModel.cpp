#include "ReminderModel.h"

ReminderModel::ReminderModel(ReminderSystem* rs, QObject* parent)
    : QAbstractListModel(parent), m_rs(rs) {
    reload();
}

int ReminderModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_reminders.size());
}

QVariant ReminderModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= m_reminders.size())
        return QVariant();

    const Reminder& r = m_reminders[index.row()];

    switch (role) {
    case IdRole: return r.id;
    case TitleRole: return QString::fromStdString(r.title);
    case TimeRole: return QString::fromStdString(r.trigger_time);
    case MessageRole: return QString::fromStdString(r.message);
    case RecurrenceRole: return QString::fromStdString(r.recurrence);
    case IsTriggeredRole: return r.triggered;
    }
    return QVariant();
}

QHash<int, QByteArray> ReminderModel::roleNames() const {
    return {
        {IdRole, "reminderId"},
        {TitleRole, "title"},
        {TimeRole, "time"},
        {MessageRole, "message"},
        {RecurrenceRole, "recurrence"},
        {IsTriggeredRole, "isTriggered"}
    };
}

void ReminderModel::addReminder(const QString& title, const QString& time) {
    addReminder(title, "", time, "once");
}

void ReminderModel::addReminder(const QString& title, const QString& message, const QString& time, const QString& recurrence) {
    // 默认一个提醒一次，允许前端指定消息与重复规则
    m_rs->addReminder(title.toStdString(), message.toStdString(), time.toStdString(), recurrence.toStdString(), 0);
    reload();
}

void ReminderModel::deleteReminder(int index) {
    if (index < 0 || index >= m_reminders.size()) return;
    m_rs->deleteReminder(m_reminders[index].id);
    reload();
}

void ReminderModel::reload() {
    beginResetModel();
    m_reminders = m_rs->getActiveReminders();
    endResetModel();
}
