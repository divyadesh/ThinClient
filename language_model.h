#ifndef LANGUAGE_MODEL_H
#define LANGUAGE_MODEL_H

#include <QAbstractListModel>
#include <QLocale>
#include <QVector>
#include <QTranslator>
#include <QQmlEngine>

struct LanguageItem {
    QString localeId;   // e.g. "en_US"
    QString displayName; // e.g. "English (United States)"
};

class LanguageModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        LocaleIdRole = Qt::UserRole + 1,
        DisplayNameRole,
        NativeNameRole,
        FullNameRole
    };

    explicit LanguageModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QVariantMap get(int index) const;

    Q_INVOKABLE void refresh();
    Q_INVOKABLE bool setSystemLanguage(const QString &locale);
signals:
    void languageChanged();

private:
    void load();
    QTranslator m_translator;
    QVector<LanguageItem> m_items;
};

#endif // LANGUAGE_MODEL_H
