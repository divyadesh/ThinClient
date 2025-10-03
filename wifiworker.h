// WifiWorker.h
#pragma once
#include <QObject>
#include <QProcess>

class WifiWorker : public QObject
{
    Q_OBJECT
public:
    explicit WifiWorker(QObject *parent = nullptr);
public slots:
    void checkConnection();
signals:
    void connectedChanged(bool connected);
};
