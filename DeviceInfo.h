#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QObject>
#include <QProcess>

class DeviceInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString cpuDetails READ cpuDetails WRITE setCpuDetails NOTIFY sigCpuDetailsChanged FINAL)
    Q_PROPERTY(QString ramSize READ ramSize WRITE setRamSize NOTIFY sigRamSizeChanged FINAL)
    Q_PROPERTY(QString gpuDetails READ gpuDetails WRITE setGpuDetails NOTIFY sigGpuDetailsChanged FINAL)

    QString m_cpuDetails{""};
    QString m_ramSize{"123KB"};
    QProcess m_process;
public:
    explicit DeviceInfo(QObject *parent = nullptr);

    QString cpuDetails() const;
    void setCpuDetails(const QString &newCpuDetails);

    QString ramSize() const;
    void setRamSize(const QString &newRamSize);

    Q_INVOKABLE void getDeviceInfoDetails();

    QString gpuDetails() const;
    void setGpuDetails(const QString &newGpuDetails);

signals:
    void sigCpuDetailsChanged(QString cpuDetails);
    void sigRamSizeChanged(QString ramSize);

    void sigGpuDetailsChanged();
private:
    QString m_gpuDetails;
};

#endif // DEVICEINFO_H
