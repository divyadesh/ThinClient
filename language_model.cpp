#include "language_model.h"
#include <QDebug>
#include <QSet>
#include <QFile>
#include<QVariant>
#include <QString>
#include <QVariantMap>
#include <QHash>
#include <QGuiApplication>
#include "Application.h"
#include "PersistData.h"

LanguageModel::LanguageModel(QObject *parent)
    : QAbstractListModel(parent)
{
    load();
}

int LanguageModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return m_items.size();
}

QVariant LanguageModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_items.size())
        return {};

    const auto &item = m_items.at(index.row());

    switch (role) {
    case LocaleIdRole:
        return item.localeId;
    case DisplayNameRole:
        return item.displayName;
    case NativeNameRole:
        return QLocale(item.localeId).nativeLanguageName();
    case FullNameRole:
        return QLocale(item.localeId).nativeLanguageName() +
               " (" + QLocale(item.localeId).nativeCountryName() + ")";
    default:
        return {};
    }
}

QHash<int, QByteArray> LanguageModel::roleNames() const {
    return {
        { LocaleIdRole, "localeId" },
        { DisplayNameRole, "displayName" },
        { NativeNameRole, "nativeName" },
        { FullNameRole, "fullDisplayName" },
    };
}

QVariantMap LanguageModel::get(int index) const {
    QVariantMap map;
    if (index < 0 || index >= m_items.size())
        return map;

    const auto &item = m_items.at(index);
    map["localeId"] = item.localeId;
    map["displayName"] = item.displayName;

    return map;
}

void LanguageModel::refresh() {
    load();
}

bool LanguageModel::setSystemLanguage(const QString &locale)
{
    qDebug() << "===== LANGUAGE CHANGE REQUESTED =====";
    qDebug() << "Requested locale:" << locale;

    // Remove previous translator
    if (!m_translator.isEmpty()) {
        qDebug() << "Existing translator found. Removing old translator...";
        bool removed = qApp->removeTranslator(&m_translator);
        qDebug() << "Old translator removed:" << removed;
    } else {
        qDebug() << "No previous translator loaded. Skipping removal.";
    }

    // Compose file path
    QString qmFile = QString(":/translations/%1.qm").arg(locale);
    qDebug() << "Translation file to load:" << qmFile;

    // Load new translator
    qDebug() << "Attempting to load translation file...";
    if (!m_translator.load(qmFile)) {
        qWarning() << "ERROR: Failed to load translation file!";
        qWarning() << "File expected at:" << qmFile;
        qWarning() << "Check if file exists in qrc and lrelease was run.";
        return false;
    }
    qDebug() << "Translation file loaded successfully.";

    // Install translator
    qDebug() << "Installing new translator...";
    bool installed = qApp->installTranslator(&m_translator);
    qDebug() << "Translator installation status:" << installed;

    // Handle RTL/LTR layout
    if (locale == "ar_SA") {
        qDebug() << "Locale is Arabic → Setting Right-To-Left layout direction.";
        QGuiApplication::setLayoutDirection(Qt::RightToLeft);
    } else {
        qDebug() << "Locale is Left-To-Right → Setting default layout direction.";
        QGuiApplication::setLayoutDirection(Qt::LeftToRight);
    }

    qDebug() << "====== LANGUAGE SWITCH FINISHED SUCCESSFULLY ======";
    qDebug() << "Active language now:" << locale;

    emit languageChanged();
    return true;
}

void LanguageModel::load() {
    beginResetModel();
    m_items.clear();

    // --- Fixed list of supported languages ---
    struct Lang {
        QString id;
        QString name;
    };

    static const QList<Lang> supported = {
        { "en_US", "English" },
        { "fr_FR", "French" },
        { "es_ES", "Spanish" },
        { "ru_RU", "Russian" },
        { "de_DE", "German" },
        { "ar_SA", "Arabic" },
        { "zh_CN", "Mandarin Chinese" }
    };

    for (const auto &lang : supported) {
        LanguageItem item;
        item.localeId = lang.id;
        item.displayName = lang.name;
        m_items.push_back(item);
    }

    // Default English first; rest sorted alphabetically
    std::sort(m_items.begin() + 1, m_items.end(),
              [](const LanguageItem &a, const LanguageItem &b) {
                  return a.displayName.localeAwareCompare(b.displayName) < 0;
              });

    endResetModel();
}
