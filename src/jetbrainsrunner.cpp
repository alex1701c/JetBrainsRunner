#include "jetbrainsrunner.h"
#include "JetbrainsApplication.h"
#include "SettingsDirectory.h"
// KF
#include <KLocalizedString>
#include <KSharedConfig>
#include <QtGui/QtGui>
#include <QDate>

JetbrainsRunner::JetbrainsRunner(QObject *parent, const QVariantList &args)
        : Plasma::AbstractRunner(parent, args) {
    setObjectName(QStringLiteral("JetbrainsRunner"));
}

JetbrainsRunner::~JetbrainsRunner() = default;

void JetbrainsRunner::init() {
    const QString configFolder = QDir::homePath() + "/.config/krunnerplugins/";
    const QDir configDir(configFolder);
    if (!configDir.exists()) configDir.mkpath(configFolder);
    // Create file
    QFile configFile(configFolder + "jetbrainsrunnerrc");
    if (!configFile.exists()) {
        configFile.open(QIODevice::WriteOnly);
        configFile.close();
    }
    // Add file watcher for config
    watcher.addPath(configFolder + "jetbrainsrunnerrc");
    connect(&watcher, SIGNAL(fileChanged(QString)), this, SLOT(reloadPluginConfiguration(QString)));
    reloadPluginConfiguration();
}


void JetbrainsRunner::reloadPluginConfiguration(const QString &configFile) {
    KConfigGroup config = KSharedConfig::openConfig(QDir::homePath() + "/.config/krunnerplugins/jetbrainsrunnerrc")
            ->group("Config");
    // Force sync from file
    if (!configFile.isEmpty()) config.config()->reparseConfiguration();

    // If the file gets edited with a text editor, it often gets replaced by the edited version
    // https://stackoverflow.com/a/30076119/9342842
    if (!configFile.isEmpty()) {
        if (QFile::exists(configFile)) {
            watcher.addPath(configFile);
        }
    }

    // General settings
    formatString = config.readEntry("FormatString");
    if (!formatString.contains("%PROJECT")) formatString = "%APPNAME launch %PROJECT";
    launchByAppName = config.readEntry("LaunchByAppName", "true") == "true";
    launchByProjectName = config.readEntry("LaunchByProjectName", "true") == "true";
    displayInCategories = config.readEntry("DisplayInCategories") == "true";
    installed.clear();
    appNameRegex = QRegExp(R"(^(\w+)(?: (.+))?$)");

    // Initialize/read settings for JetbrainsApplications
    const auto mappingMap = config.group("CustomMapping").entryMap();
    QList<JetbrainsApplication *> appList;
    QList<JetbrainsApplication *> automaticAppList;
    const auto desktopPaths = JetbrainsApplication::getInstalledApplicationPaths(config.group("CustomMapping"));

    // Split manually configured and automatically found apps
    for (const auto &p:desktopPaths.toStdMap()) {
        // Desktop file is manually specified
        if (mappingMap.contains(p.second)) {
            auto customMappedApp = new JetbrainsApplication(p.second);
            QFile xmlConfigFile(mappingMap.value(p.second));
            if (xmlConfigFile.open(QFile::ReadOnly)) {
                customMappedApp->parseXMLFile(xmlConfigFile.readAll());
                // Add path for filewatcher
                customMappedApp->addPath(mappingMap.value(p.second));
                if (!customMappedApp->recentlyUsed.isEmpty()) {
                    appList.append(customMappedApp);
                }
            }
            xmlConfigFile.close();
        } else {
            automaticAppList.append(new JetbrainsApplication(p.second));
        }
    }

    // Find automatically config directory, read config file and filter apps
    SettingsDirectory::findCorrespondingDirectories(SettingsDirectory::getSettingsDirectories(), automaticAppList);
    JetbrainsApplication::parseXMLFiles(automaticAppList);
    automaticAppList = JetbrainsApplication::filterApps(automaticAppList);
    appList.append(automaticAppList);
    installed = appList;

    // Version update notification
    QDate lastUpdateCheckDate = QDate::fromString(config.readEntry("checkedUpdateDate"));
    if (config.readEntry("NotifyUpdates", "true") == "true" && (
            !lastUpdateCheckDate.isValid() ||
            lastUpdateCheckDate.addDays(7) <= QDate::currentDate())) {
        auto manager = new QNetworkAccessManager(this);
        QNetworkRequest request(QUrl("https://api.github.com/repos/alex1701c/JetBrainsRunner/releases"));
        manager->get(request);
        connect(manager, SIGNAL(finished(QNetworkReply * )), this, SLOT(displayUpdateNotification(QNetworkReply * )));
        config.writeEntry("checkedUpdateDate", QDate::currentDate().toString());
    }
}

void JetbrainsRunner::match(Plasma::RunnerContext &context) {
    const QString term = context.query().toLower();
    QList<Plasma::QueryMatch> matches;

    if (launchByAppName) {
        matches.append(addAppNameMatches(term));
    }
    if (launchByProjectName) {
        matches.append(addProjectNameMatches(term));
    }
    context.addMatches(matches);
}

void JetbrainsRunner::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) {
    Q_UNUSED(context)

    QProcess::startDetached(match.data().toString());
}

QList<Plasma::QueryMatch> JetbrainsRunner::addAppNameMatches(const QString &term) {
    QList<Plasma::QueryMatch> matches;

    appNameRegex.indexIn(term);
    QString termName = appNameRegex.capturedTexts().at(1);
    if (termName.isEmpty()) return matches;
    const QString termProject = appNameRegex.capturedTexts().at(2);

    for (auto const &app:installed) {
        if (app->nameArray[0].startsWith(termName, Qt::CaseInsensitive) ||
            (!app->nameArray[1].isEmpty() && app->nameArray[1].startsWith(termName, Qt::CaseInsensitive))
                ) {
            const int recentProjectsCount = app->recentlyUsed.size();
            for (int i = 0; i < recentProjectsCount; ++i) {
                const auto &dir = app->recentlyUsed.at(i);
                const QString dirName = dir.split('/').last();
                if (termProject.isEmpty() || dirName.startsWith(termProject, Qt::CaseInsensitive)) {
                    Plasma::QueryMatch match(this);
                    match.setText(QString(formatString)
                                          .replace("%PROJECT", dirName)
                                          .replace("%APPNAME", app->name)
                                          .replace("%APP", app->shortName)
                    );
                    match.setIconName(app->iconPath);
                    match.setData(QString(app->executablePath).replace("%u", dir).replace("%f", dir));
                    match.setRelevance((float) 1 / (float) (i + 1));
                    if(displayInCategories) match.setMatchCategory(app->name);
                    matches.append(match);
                }
            }
        }
    }

    return matches;
}

QList<Plasma::QueryMatch> JetbrainsRunner::addProjectNameMatches(const QString &term) {
    QList<Plasma::QueryMatch> matches;
    for (auto const &app:installed) {
        // If the plugin displays search suggestions by appname and the application name matches the search
        // term the options have already been created in the addAppNameMatches method
        // => this app should be skipped to avoid duplicates
        if ((launchByAppName &&
             app->nameArray[0].startsWith(term, Qt::CaseInsensitive)) ||
            (!app->nameArray[1].isEmpty() && app->nameArray[1].startsWith(term, Qt::CaseInsensitive))
                ) {
            continue;
        }
        for (const auto &dir:app->recentlyUsed) {
            const QString dirName = dir.split('/').last();
            if (dirName.startsWith(term, Qt::CaseInsensitive)) {
                Plasma::QueryMatch match(this);
                match.setText(QString(formatString)
                                      .replace("%PROJECT", dirName)
                                      .replace("%APPNAME", app->name)
                                      .replace("%APP", app->shortName)
                );
                match.setIconName(app->iconPath);
                match.setData(QString(app->executablePath).replace("%u", dir).replace("%f", dir));
                if(displayInCategories) match.setMatchCategory(app->name);
                matches.append(match);
            }
        }
    }
    return matches;
}

void JetbrainsRunner::displayUpdateNotification(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QString displayText;
        const auto jsonObject = QJsonDocument::fromJson(reply->readAll());
        if (jsonObject.isArray()) {
            for (const auto &githubReleaseObj:jsonObject.array()) {
                if (githubReleaseObj.isObject()) {
                    const auto githubRelease = githubReleaseObj.toObject();
                    if (githubRelease.value("tag_name").toString() > "1.3.1") {
                        displayText.append(githubRelease.value("tag_name").toString() + ": " +
                                           githubRelease.value("name").toString() + "\n");
                    }
                }
            }
        }
        if (!displayText.isEmpty()) {
            displayText.prepend("New Versions Available:\n");
            displayText.append("Please go to https://github.com/alex1701c/JetBrainsRunner</a>");
            QProcess::startDetached("notify-send", QStringList{
                    "JetBrains Runner Updates!", displayText, "--icon", "jetbrains", "--expire-time", "5000"
            });
        }
    }
}

K_EXPORT_PLASMA_RUNNER(jetbrainsrunner, JetbrainsRunner)

// needed for the QObject subclass declared as part of K_EXPORT_PLASMA_RUNNER
#include "jetbrainsrunner.moc"
