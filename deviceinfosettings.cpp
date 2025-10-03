#include "deviceinfosettings.h"

DeviceInfoSettings::DeviceInfoSettings(QObject *parent)
    : QObject(parent)
{}

bool DeviceInfoSettings::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
        return false;

    QJsonObject obj = doc.object();

    m_developedBy = obj.value("developedBy").toString();
    m_model = obj.value("model").toString();
    m_cpu = obj.value("cpu").toString();
    m_ram = obj.value("ram").toString();
    m_gpu = obj.value("gpu").toString();
    m_ethernet = obj.value("ethernet").toString();
    m_wifi = obj.value("wifi").toString();
    m_firmwareVersion = obj.value("firmwareVersion").toString();
    m_madeIn = obj.value("madeIn").toString();

    emit infoChanged();
    return true;
}
