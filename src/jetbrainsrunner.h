#ifndef JETBRAINSRUNNER_H
#define JETBRAINSRUNNER_H

#include <KRunner/AbstractRunner>
#include <QtNetwork/QNetworkReply>
#include <QDir>
#include <QRegularExpression>
#include "jetbrains-api/JetbrainsApplication.h"
#include "jetbrains-api/ConfigKeys.h"
#include "krunner_version.h"

class JetbrainsRunner : public Plasma::AbstractRunner {
Q_OBJECT

public:
#if KRUNNER_VERSION >= QT_VERSION_CHECK(5, 77, 0)
    JetbrainsRunner(QObject *parent, const KPluginMetaData &pluginMetaData, const QVariantList &args)
        : Plasma::AbstractRunner(parent, pluginMetaData, args) {
        setObjectName(QStringLiteral("JetbrainsRunner"));
    }
#else
    JetbrainsRunner(QObject *parent, const QVariantList &args)
    : Plasma::AbstractRunner(parent, args) {
        setObjectName(QStringLiteral("JetbrainsRunner"));
    }
#endif
    ~JetbrainsRunner() override;

    QFileSystemWatcher watcher;
    bool launchByAppName, launchByProjectName, displayInCategories;
    QString formatString;
    SearchResultChoice searchResultChoice;
    QRegularExpression appNameRegex;
    QList<JetbrainsApplication *> installed;
    const QChar sep = QDir::separator();

    inline bool projectMatchesQuery(const QString &term, const Project &project) const;

    QList<Plasma::QueryMatch> addAppNameMatches(const QString &term);
    QList<Plasma::QueryMatch> addProjectNameMatches(const QString &term);
    QList<Plasma::QueryMatch> addPathNameMatches(const QString &term);

protected Q_SLOTS:

    void init() override;
    void reloadPluginConfiguration(const QString &configFile = QString());
    static void displayUpdateNotification(QNetworkReply *reply);
    static void writeDesktopFile(const Plasma::QueryMatch &match, const QString &filePath);

public: // Plasma::AbstractRunner API
    void match(Plasma::RunnerContext &context) override;
    QMimeData *mimeDataForMatch(const Plasma::QueryMatch &match) override;
    void run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) override;
};

#endif
