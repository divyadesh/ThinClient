#include "wifiworker.h"
#include <QTimer>

WifiWorker::WifiWorker(QObject *parent)
    : QObject(parent)
{
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &WifiWorker::checkConnection);
    timer->start(2000);
}

void WifiWorker::checkConnection()
{
    QProcess process;
    process.start("nmcli -t -f STATE g");
    process.waitForFinished(1000);
    QString state = process.readAllStandardOutput().trimmed();
    bool connected = (state == "connected");
    emit connectedChanged(connected);
}
