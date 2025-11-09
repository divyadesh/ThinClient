#pragma once
#include <QDateTime>
#include <QFile>
#include <QMutex>
#include <QTextStream>
#include <QDebug>

#define LOGDebug() qDebug().noquote().nospace()
#define LOGInfo() qInfo().noquote().nospace()
#define LOGWarning() qWarning().noquote().nospace()
#define LOGError() qCritical().noquote().nospace()

class Logger {
public:
    static void init(const QString &filePath = "/var/log/thinclient.log");
    static void shutdown();

private:
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);
    static QFile logFile;
    static QMutex mutex;
};
