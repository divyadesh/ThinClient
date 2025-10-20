#include "PersistData.h"
#include <QDebug>
PersistData::PersistData(QObject *parent)
    : QObject{parent} {
}

PersistData::~PersistData() {

}

void PersistData::saveData(const QString& key, const QString& value) {
    m_setting.beginGroup("ThinClient");
    m_setting.setValue(key, value);
    m_setting.endGroup();
    qDebug()<<"::::> qsetting file: "<<m_setting.fileName()<<", key="<<key<<", value="<<value;
}

QString PersistData::getData(const QString &key) {
    QString value;
    m_setting.beginGroup("ThinClient");
    value = m_setting.value(key).toString();
    m_setting.endGroup();
    return value;
}

//QProcess::execute("xrandr --output HDMI-1 --mode 1920x1080");
//QProcess::execute("timedatectl set-timezone Europe/London");

/*
 *
void switchAudioSink(const QString &sinkName) {
    // Set default sink
    QProcess::execute(QString("pactl set-default-sink %1").arg(sinkName));

    // Move active streams
    QProcess::execute(QString(
        "for i in $(pactl list short sink-inputs | awk '{print $1}'); "
        "do pactl move-sink-input $i %1; done").arg(sinkName));

    qDebug() << "Switched to" << sinkName;
}


    addTab("HDMI", "alsa_output.pci-0000_01_00.1.hdmi-stereo");
    addTab("USB Headset", "alsa_output.usb-Logitech_USB_Headset-00.analog-stereo");
    addTab("Analog (Jack)", "alsa_output.pci-0000_00_1b.0.analog-stereo");

 */
