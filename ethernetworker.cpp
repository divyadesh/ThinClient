#include "ethernetworker.h"

EthernetWorker::EthernetWorker(QObject *parent)
    : QObject(parent)
{}

void EthernetWorker::checkConnection()
{
    QProcess process;
    process.start("nmcli -t -f TYPE,STATE con show --active");
    process.waitForFinished(1000);

    QString output = process.readAllStandardOutput().trimmed();
    bool connected = output.contains("ethernet:activated");
    emit connectedChanged(connected);
}
