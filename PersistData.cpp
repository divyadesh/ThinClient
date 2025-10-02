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
