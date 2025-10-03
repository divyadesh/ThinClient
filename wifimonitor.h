#pragma once
#include <QObject>

class WifiMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    explicit WifiMonitor(QObject *parent = nullptr);

    bool isConnected() const;

public slots:
    void setConnected(bool connected); // called via queued connection

signals:
    void connectedChanged(bool connected);

private:
    bool m_connected;
};
