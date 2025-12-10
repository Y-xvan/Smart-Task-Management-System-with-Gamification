#ifndef REMINDERMODEL_H
#define REMINDERMODEL_H

#include <QAbstractListModel>
#include <vector>
#include "reminder/ReminderSystem.h"

class ReminderModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum ReminderRoles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        TimeRole,
        MessageRole,
        RecurrenceRole,
        IsTriggeredRole
    };

    ReminderModel(ReminderSystem* rs, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addReminder(const QString& title, const QString& time);
    Q_INVOKABLE void addReminder(const QString& title, const QString& message, const QString& time, const QString& recurrence);
    Q_INVOKABLE void deleteReminder(int index);
    Q_INVOKABLE void reload();

private:
    ReminderSystem* m_rs;
    std::vector<Reminder> m_reminders;
};
#endif // REMINDERMODEL_H
