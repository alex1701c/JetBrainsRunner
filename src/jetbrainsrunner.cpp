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
    config = KSharedConfig::openConfig("krunnerrc")->group("Runners").group("JetBrainsRunner");
    reloadConfiguration();
}


void JetbrainsRunner::reloadConfiguration() {
    // General settings
    launchByAppName = config.readEntry("LaunchByAppName", "true") == "true";
    launchByProjectName = config.readEntry("LaunchByProjectName", "true") == "true";
    installed.clear();
    appNameRegex = QRegExp(R"(^(\w+)(?: (.+))?$)");

    // Initialize/read settings for JetbrainsApplications
    const auto mappingMap = config.group("CustomMapping").entryMap();
    QList<JetbrainsApplication *> appList;
    QList<JetbrainsApplication *> automaticAppList;
    auto desktopPaths = JetbrainsApplication::getInstalledApplicationPaths(config.group("CustomMapping"));

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
    QString termProject = appNameRegex.capturedTexts().at(2);

    for (auto const &app:installed) {
        if (app->nameArray[0].startsWith(termName, Qt::CaseInsensitive)
            || (app->secondName && app->nameArray[1].startsWith(termName, Qt::CaseInsensitive))
                ) {
            const int recentProjectsCount = app->recentlyUsed.size();
            for (int i = 0; i < recentProjectsCount; ++i) {
                const auto &dir = app->recentlyUsed.at(i);
                if (termProject.isEmpty() || dir.split('/').last().startsWith(termProject, Qt::CaseInsensitive)) {
                    Plasma::QueryMatch match(this);
                    match.setText(app->name + " launch " + dir.split('/').last());
                    match.setIconName(app->iconPath);
                    match.setData(QString(app->executablePath).replace("%f", dir));
                    match.setRelevance((float) 1 / (float) (i + 1));
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
        for (const auto &dir:app->recentlyUsed) {
            if (dir.split('/').last().startsWith(term, Qt::CaseInsensitive)) {
                Plasma::QueryMatch match(this);
                match.setText(" Launch " + dir.split('/').last() + " in " + app->name);
                match.setIconName(app->iconPath);
                match.setData(QString(app->executablePath).replace("%f", dir));
                matches.append(match);
            }
        }
    }
    return matches;
}

void JetbrainsRunner::displayUpdateNotification(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QString displayText;
        auto jsonObject = QJsonDocument::fromJson(reply->readAll());
        if (jsonObject.isArray()) {
            for (const auto &githubReleaseObj:jsonObject.array()) {
                if (githubReleaseObj.isObject()) {
                    auto githubRelease = githubReleaseObj.toObject();
                    if (githubRelease.value("tag_name").toString() > "1.2.1") {
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
                    "JetBrains Runner Updates!", displayText, "--icon", "/usr/share/icons/jetbrains.png", "--expire-time", "5000"
            });
        }
        config.writeEntry("checkedUpdateDate", QDate::currentDate().toString());
    }
}

K_EXPORT_PLASMA_RUNNER(jetbrainsrunner, JetbrainsRunner)

// needed for the QObject subclass declared as part of K_EXPORT_PLASMA_RUNNER
#include "jetbrainsrunner.moc"
