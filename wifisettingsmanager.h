#include <QObject>
#include <QSettings>
#include <QDebug>

class WiFiSettingsManager : public QObject {
    Q_OBJECT
public:
    explicit WiFiSettingsManager(QObject *parent = nullptr);

    Q_INVOKABLE void savePassword(const QString &ssid, const QString &password);
    Q_INVOKABLE QString getPassword(const QString &ssid);
    Q_INVOKABLE bool hasSavedPassword(const QString &ssid);
    Q_INVOKABLE void clearPassword(const QString &ssid); // optional

private:
    QSettings m_settings;
    QString m_group = "WiFiPasswords";

    void logChange(const QString &ssid, const QString &action);
};

