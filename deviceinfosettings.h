#ifndef DEVICEINFOSETTINGS_H
#define DEVICEINFOSETTINGS_H

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QProcess>

class DeviceInfoSettings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString developedBy READ developedBy NOTIFY infoChanged)
    Q_PROPERTY(QString model READ model NOTIFY infoChanged)
    Q_PROPERTY(QString cpu READ cpu NOTIFY infoChanged)
    Q_PROPERTY(QString ram READ ram NOTIFY infoChanged)
    Q_PROPERTY(QString gpu READ gpu NOTIFY infoChanged)
    Q_PROPERTY(QString ethernet READ ethernet NOTIFY infoChanged)
    Q_PROPERTY(QString wifi READ wifi NOTIFY infoChanged)
    Q_PROPERTY(QString firmwareVersion READ firmwareVersion NOTIFY infoChanged)
    Q_PROPERTY(QString madeIn READ madeIn NOTIFY infoChanged)

public:
    explicit DeviceInfoSettings(QObject *parent = nullptr);

    QString developedBy() const { return m_developedBy; }
    QString model() const { return m_model; }
    QString cpu() const { return m_cpu; }
    QString ram() const { return m_ram; }
    QString gpu() const { return m_gpu; }
    QString ethernet() const { return m_ethernet; }
    QString wifi() const { return m_wifi; }
    QString firmwareVersion() const { return m_firmwareVersion; }
    QString madeIn() const { return m_madeIn; }

    Q_INVOKABLE bool loadFromFile(const QString &filePath);
    Q_INVOKABLE void readBoardInfo();

signals:
    void infoChanged();

private:
    QString m_developedBy;
    QString m_model;
    QString m_cpu;
    QString m_ram;
    QString m_gpu;
    QString m_ethernet;
    QString m_wifi;
    QString m_firmwareVersion;
    QString m_madeIn;

    QString execCommand(const QString &cmd);
};

#endif // DEVICEINFOSETTINGS_H
