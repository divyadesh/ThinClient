#include "ethernetworker.h"
#include <QFile>
#include <QTextStream>
#include <QTimer>

EthernetWorker::EthernetWorker(QObject *parent)
    : QObject(parent)
{
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &EthernetWorker::checkConnection);
    timer->start(2000);
}

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
