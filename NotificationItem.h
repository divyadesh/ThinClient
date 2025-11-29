#ifndef NOTIFICATIONITEM_H
#define NOTIFICATIONITEM_H

#include <QObject>

class NotificationItem : public QObject
{
    Q_OBJECT

public:
    explicit NotificationItem(QObject *parent = nullptr) : QObject(parent) {}

    // Expose to QML as Type.*
    enum Type {
        Success,
        Warning,
        Info,
        Error
    };
    Q_ENUM(Type)  // <-- Expose enum to QML
};

#endif // NOTIFICATIONITEM_H
