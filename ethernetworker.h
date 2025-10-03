// EthernetWorker.h
#pragma once
#include <QObject>
#include <QProcess>

class EthernetWorker : public QObject
{
    Q_OBJECT
public:
    explicit EthernetWorker(QObject *parent = nullptr);

public slots:
    void checkConnection(); // can be called repeatedly via QTimer

signals:
    void connectedChanged(bool connected);
};
