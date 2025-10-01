#ifndef IMAGEUPDATER_H
#define IMAGEUPDATER_H

#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>

class ImageUpdater : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool updating READ isUpdating NOTIFY updatingChanged)

public:
    explicit ImageUpdater(QObject *parent = nullptr);
    Q_INVOKABLE void startUpdate(const QString &url);
    Q_INVOKABLE void cancelUpdate();

    bool isUpdating() const;

signals:
    void progressChanged(const QString &step, int percent);
    void errorOccurred(const QString &errorMessage);
    void rebootCountdown(int secondsLeft);
    void updatingChanged();

private slots:
    void handleDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void handleDownloadFinished();
    void handleDownloadError(QNetworkReply::NetworkError code);

private:
    void startRebootCountdown();
    void resetState();

    QString m_targetPath;
    QNetworkAccessManager m_networkManager;
    QNetworkReply *m_reply = nullptr;
    QFile m_file;
    bool m_updating = false;
};

#endif // IMAGEUPDATER_H
