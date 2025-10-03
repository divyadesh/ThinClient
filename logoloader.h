// LogoLoader.h
#pragma once
#include <QObject>
#include <QString>
#include <QFile>

class LogoLoader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString logo READ logo NOTIFY logoChanged)

public:
    explicit LogoLoader(QObject *parent = nullptr);

    QString logo() const;

signals:
    void logoChanged();

private:
    QString m_logo;
    void loadLogo();
};
