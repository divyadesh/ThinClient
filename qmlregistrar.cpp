#include "qmlregistrar.h"
#include <QQmlContext>
#include <QTimer>

QmlRegistrar::QmlRegistrar(QQmlApplicationEngine *engine, QObject *parent)
    : QObject(parent), m_engine(engine)
{
    setupNetworkMonitors();
    setupLogoLoader();

    m_dnsNetworkInfo = new DNSNetworkInfo();
}

QmlRegistrar::~QmlRegistrar()
{
    if (m_wifiThread) {
        m_wifiThread->quit();
        m_wifiThread->wait();
    }
    if (m_ethThread) {
        m_ethThread->quit();
        m_ethThread->wait();
    }
}

void QmlRegistrar::setupNetworkMonitors()
{
    // --- Wi-Fi ---
    m_wifiMonitor = new WifiMonitor();           // main-thread proxy
    m_wifiWorker = new WifiWorker();      // background worker
    m_wifiThread = new QThread();
    m_wifiWorker->moveToThread(m_wifiThread);

    connect(m_wifiThread, &QThread::started, m_wifiWorker, &WifiWorker::checkConnection);
    connect(m_wifiWorker, &WifiWorker::connectedChanged,
            m_wifiMonitor, &WifiMonitor::setConnected, Qt::QueuedConnection);
    connect(m_wifiThread, &QThread::finished, m_wifiWorker, &QObject::deleteLater);
    m_wifiThread->start();

    // --- Ethernet ---
    m_ethMonitor = new EthernetMonitor();
    m_ethWorker = new EthernetWorker();
    m_ethThread = new QThread();
    m_ethWorker->moveToThread(m_ethThread);

    connect(m_ethThread, &QThread::started, m_ethWorker, &EthernetWorker::checkConnection);
    connect(m_ethWorker, &EthernetWorker::connectedChanged,
            m_ethMonitor, &EthernetMonitor::setConnected, Qt::QueuedConnection);
    connect(m_ethThread, &QThread::finished, m_ethWorker, &QObject::deleteLater);
    m_ethThread->start();
}

void QmlRegistrar::setupLogoLoader()
{
    m_logoLoader = new LogoLoader(this);  // main thread, safe for QML
}

void QmlRegistrar::registerTypesAndContext()
{
    // Expose proxies to QML
    m_engine->rootContext()->setContextProperty("wifiMonitor", m_wifiMonitor);
    m_engine->rootContext()->setContextProperty("ethernetMonitor", m_ethMonitor);
    m_engine->rootContext()->setContextProperty("logoLoader", m_logoLoader);
    m_engine->rootContext()->setContextProperty("dnsNetworkInfo", m_dnsNetworkInfo);
}
