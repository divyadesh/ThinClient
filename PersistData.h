#ifndef PERSISTDATA_H
#define PERSISTDATA_H

#include <QObject>
#include <QSettings>

class PersistData : public QObject
{
    Q_OBJECT
    QSettings m_setting;
public:
    explicit PersistData(QObject *parent = nullptr);
    ~PersistData();
    Q_INVOKABLE void saveData(const QString& key, const QString& value);
    Q_INVOKABLE QString getData(const QString& key);

signals:
};

#endif // PERSISTDATA_H
