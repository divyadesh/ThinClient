#pragma once

#include <QObject>

class EthernetMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected WRITE setIsConnected NOTIFY connectedChanged)


public:
    explicit EthernetMonitor(QObject *parent = nullptr);
    bool isConnected() const;

public slots:
    void setIsConnected(bool newConnected);

signals:
    void connectedChanged();

private:
    bool m_connected;
};
