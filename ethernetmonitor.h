// EthernetMonitor.h
#pragma once
#include <QObject>

class EthernetMonitor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)

public:
    explicit EthernetMonitor(QObject *parent = nullptr);

    bool isConnected() const;

public slots:
    void setConnected(bool connected);

signals:
    void connectedChanged(bool connected);

private:
    bool m_connected;
};
