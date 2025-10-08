#ifndef RDPMONITORTHREAD_H
#define RDPMONITORTHREAD_H
#include <QThread>
#include <QProcess>
#include <QDebug>

class RdpMonitorThread : public QThread
{
    Q_OBJECT
public:
    RdpMonitorThread(QObject *parent = nullptr) : QThread(parent) {}

    void run() override {
        // Wait until wlfreerdp process exits
        while (true) {
            QThread::sleep(5); // poll interval
            bool rdpRunning = !QProcess::execute("pgrep", {"wlfreerdp"});
            if (!rdpRunning) {
                qDebug() << "RDP not running, killing Weston...";
                QProcess::execute("pkill", {"weston"});
                break;
            }
        }
    }
};

#endif // RDPMONITORTHREAD_H
