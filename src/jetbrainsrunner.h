#ifndef JETBRAINSRUNNER_H
#define JETBRAINSRUNNER_H

#include <KRunner/AbstractRunner>
#include <QtNetwork/QNetworkReply>
#include "jetbrains-api/JetbrainsApplication.h"

class JetbrainsRunner : public Plasma::AbstractRunner {
Q_OBJECT

public:
    JetbrainsRunner(QObject *parent, const QVariantList &args);
    ~JetbrainsRunner() override;

    QFileSystemWatcher watcher;
    bool launchByAppName, launchByProjectName, displayInCategories;
    QString formatString;
    QRegularExpression appNameRegex;
    QList<JetbrainsApplication *> installed;
    const QChar sep = QDir::separator();

    QList<Plasma::QueryMatch> addAppNameMatches(const QString &term);
    QList<Plasma::QueryMatch> addProjectNameMatches(const QString &term);

protected Q_SLOTS:

    void init() override;
    void reloadPluginConfiguration(const QString &configFile = "");
    static void displayUpdateNotification(QNetworkReply *reply);
    static void writeDesktopFile(const Plasma::QueryMatch &match, const QString &filePath);

public: // Plasma::AbstractRunner API
    void match(Plasma::RunnerContext &context) override;
    QMimeData *mimeDataForMatch(const Plasma::QueryMatch &match) override;
    void run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) override;
};

#endif
