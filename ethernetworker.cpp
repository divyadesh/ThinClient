#include "ethernetworker.h"
#include <QFile>
#include <QTextStream>
EthernetWorker::EthernetWorker(QObject *parent)
    : QObject(parent)
{}

void EthernetWorker::checkConnection()
{
    bool connected = false;
    QFile carrierFile("/sys/class/net/eth0/carrier");

    if (carrierFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&carrierFile);
        QString line = in.readLine().trimmed();
        connected = (line == "1");
        carrierFile.close();
    }

    emit connectedChanged(connected);
}
