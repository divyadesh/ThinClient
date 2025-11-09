#include "logger.h"

QFile Logger::logFile;
QMutex Logger::mutex;

void Logger::init(const QString &filePath)
{
    QMutexLocker lock(&mutex);

    logFile.setFileName(filePath);
    if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Logger: Cannot open log file" << filePath;
        return;
    }

    QTextStream out(&logFile);
    out << "\n=== Log started at " << QDateTime::currentDateTime().toString(Qt::ISODate) << " ===\n";
    out.flush();

    qInstallMessageHandler(Logger::messageHandler);
}

void Logger::shutdown()
{
    QMutexLocker lock(&mutex);
    if (logFile.isOpen()) {
        QTextStream out(&logFile);
        out << "=== Log ended at " << QDateTime::currentDateTime().toString(Qt::ISODate) << " ===\n";
        out.flush();
        logFile.close();
    }
    qInstallMessageHandler(nullptr);
}

void Logger::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QMutexLocker lock(&mutex);

    QString level;
    switch (type) {
    case QtDebugMsg: level = "DEBUG"; break;
    case QtInfoMsg: level = "INFO"; break;
    case QtWarningMsg: level = "WARN"; break;
    case QtCriticalMsg: level = "CRIT"; break;
    case QtFatalMsg: level = "FATAL"; break;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString contextStr = QString("%1:%2").arg(context.file ? context.file : "?").arg(context.line);

    QString formatted = QString("[%1] [%2] [%3] %4")
                            .arg(timestamp, level, contextStr, msg);

    // Console output
    fprintf(stdout, "%s\n", formatted.toLocal8Bit().constData());
    fflush(stdout);

    // File output
    if (logFile.isOpen()) {
        QTextStream out(&logFile);
        out << formatted << "\n";
        out.flush();
    }

    if (type == QtFatalMsg)
        abort();
}
