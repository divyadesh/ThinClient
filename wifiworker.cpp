#include "wifiworker.h"

WifiWorker::WifiWorker(QObject *parent)
    : QObject(parent)
{}

void WifiWorker::checkConnection()
{
    QProcess process;
    process.start("nmcli -t -f STATE g");
    process.waitForFinished(1000);
    QString state = process.readAllStandardOutput().trimmed();
    bool connected = (state == "connected");
    emit connectedChanged(connected);
}
