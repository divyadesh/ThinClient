#pragma once
#include <QObject>
#include <QQmlApplicationEngine>
#include <QThread>
#include "wifimonitor.h"
#include "ethernetmonitor.h"
#include "logoloader.h"
#include "wifiworker.h"
#include "ethernetworker.h"
#include "dnsnetworkinfo.h"

class QmlRegistrar : public QObject
{
    Q_OBJECT
public:
    explicit QmlRegistrar(QQmlApplicationEngine *engine, QObject *parent = nullptr);
    ~QmlRegistrar();

    void registerTypesAndContext();

private:
    QQmlApplicationEngine *m_engine;

    // Proxies (main-thread objects for QML)
    WifiMonitor *m_wifiMonitor;
    EthernetMonitor *m_ethMonitor;
    LogoLoader *m_logoLoader;

    // Workers (background threads)
    WifiWorker *m_wifiWorker;
    EthernetWorker *m_ethWorker;
    DNSNetworkInfo *m_dnsNetworkInfo;

    QThread *m_wifiThread;
    QThread *m_ethThread;

    void setupNetworkMonitors();
    void setupLogoLoader();
};
