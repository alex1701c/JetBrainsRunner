#ifndef JETBRAINSRUNNER_H
#define JETBRAINSRUNNER_H

#include "jetbrains-api/ConfigKeys.h"
#include "jetbrains-api/JetbrainsApplication.h"
#include <KRunner/AbstractRunner>
#include <QDir>
#include <QRegularExpression>
#include <QtNetwork/QNetworkReply>
#include <krunner_version.h>

class JetbrainsRunner : public KRunner::AbstractRunner
{
    Q_OBJECT

public:
    JetbrainsRunner(QObject *parent, const KPluginMetaData &pluginMetaData, const QVariantList &args)
#if QT_VERSION_MAJOR == 6
        : KRunner::AbstractRunner(parent, pluginMetaData)
    {
        Q_UNUSED(args)
#else
        : KRunner::AbstractRunner(parent, pluginMetaData, args)
    {
        setObjectName(QStringLiteral("JetbrainsRunner"));
#endif
    }
    ~JetbrainsRunner() override;

    bool launchByAppName, launchByProjectName, displayInCategories;
    QString formatString;
    SearchResultChoice searchResultChoice;
    QRegularExpression appNameRegex;
    QList<JetbrainsApplication *> installed;
    const QChar sep = QDir::separator();

    inline bool projectMatchesQuery(const QString &term, const Project &project) const;

    QList<KRunner::QueryMatch> addAppNameMatches(const QString &term);
    QList<KRunner::QueryMatch> addProjectNameMatches(const QString &term);
    QList<KRunner::QueryMatch> addPathNameMatches(const QString &term);

protected Q_SLOTS:
    static void displayUpdateNotification(QNetworkReply *reply);
    static void writeDesktopFile(const KRunner::QueryMatch &match, const QString &filePath);

public: // KRunner::AbstractRunner API
    void reloadConfiguration() override;
    void match(KRunner::RunnerContext &context) override;
    QMimeData *mimeDataForMatch(const KRunner::QueryMatch &match) override;
    void run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match) override;
};

#endif
