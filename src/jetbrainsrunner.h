#ifndef JETBRAINSRUNNER_H
#define JETBRAINSRUNNER_H

#include <KRunner/AbstractRunner>
#include <QtNetwork/QNetworkReply>
#include "JetbrainsApplication.h"

class JetbrainsRunner : public Plasma::AbstractRunner {
Q_OBJECT

public:
    JetbrainsRunner(QObject *parent, const QVariantList &args);

    ~JetbrainsRunner() override;

    QFileSystemWatcher watcher;
    bool launchByAppName, launchByProjectName, displayInCategories;
    QString formatString;
    QRegExp appNameRegex;

    QList<JetbrainsApplication *> installed;

    QList<Plasma::QueryMatch> addAppNameMatches(const QString &term);

    QList<Plasma::QueryMatch> addProjectNameMatches(const QString &term);

protected Q_SLOTS:

    void init() override;

    void reloadPluginConfiguration(const QString &configFile = "");

    void displayUpdateNotification(QNetworkReply *reply);

public: // Plasma::AbstractRunner API
    void match(Plasma::RunnerContext &context) override;

    void run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) override;
};

#endif
