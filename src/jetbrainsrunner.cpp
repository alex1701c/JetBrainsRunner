#include "jetbrainsrunner.h"
#include "jetbrains-api/JetbrainsApplication.h"
#include "jetbrains-api/SettingsDirectory.h"
#include "jetbrains-api/ConfigKeys.h"
#include <KLocalizedString>
#include <KSharedConfig>
#include <QtGui/QtGui>
#include <QDate>
#include <QStringBuilder>

#include "jetbrains-api/export.h"

JetbrainsRunner::JetbrainsRunner(QObject *parent, const QVariantList &args)
        : Plasma::AbstractRunner(parent, args) {
    setObjectName(QStringLiteral("JetbrainsRunner"));
}

JetbrainsRunner::~JetbrainsRunner() {
    qDeleteAll(installed);
}

void JetbrainsRunner::init() {
    const QString configFolder = QDir::homePath() + QStringLiteral("/.config/krunnerplugins/");
    const QDir configDir(configFolder);
    if (!configDir.exists()) configDir.mkpath(configFolder);
    // Create file
    QFile configFile(configFolder + QStringLiteral("jetbrainsrunnerrc"));
    if (!configFile.exists()) {
        configFile.open(QIODevice::WriteOnly);
        configFile.close();
    }
    // Add file watcher for config
    watcher.addPath(configFile.fileName());
    connect(&watcher, &QFileSystemWatcher::fileChanged, this, &JetbrainsRunner::reloadPluginConfiguration);
    reloadPluginConfiguration();
}


void JetbrainsRunner::reloadPluginConfiguration(const QString &configFile) {
    KConfigGroup config = KSharedConfig::openConfig(
            QDir::homePath() % QStringLiteral("/.config/krunnerplugins/jetbrainsrunnerrc"))
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
    formatString = config.readEntry(Config::formatString);
    // Replace invalid string with default
    if (!formatString.contains(QLatin1String(FormatString::DIR)) && !formatString.contains(QLatin1String(FormatString::PROJECT))) {
        formatString = Config::formatStringDefault;
    }
    launchByAppName = config.readEntry(Config::launchByAppName, true);
    launchByProjectName = config.readEntry(Config::launchByProjectName, true);
    displayInCategories = config.readEntry(Config::displayInCategories, false);
    qDeleteAll(installed);
    installed.clear();
    if (launchByAppName) {
        appNameRegex = QRegularExpression(R"(^(\w+)(?: (.+))?$)");
        appNameRegex.optimize();
    }

    installed = JetbrainsAPI::fetchApplications(config);

    // Version update notification
    QDate lastUpdateCheckDate = QDate::fromString(config.readEntry(Config::checkedUpdateDate));
    if (config.readEntry(Config::notifyUpdates, true) && (
            !lastUpdateCheckDate.isValid() ||
            lastUpdateCheckDate.addDays(7) <= QDate::currentDate())) {
        auto manager = new QNetworkAccessManager(this);
        QNetworkRequest request(QUrl(QStringLiteral("https://api.github.com/repos/alex1701c/JetBrainsRunner/releases")));
        manager->get(request);
        connect(manager, &QNetworkAccessManager::finished, this, &JetbrainsRunner::displayUpdateNotification);
        config.writeEntry(Config::checkedUpdateDate, QDate::currentDate().toString());
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

    const auto regexMatch = appNameRegex.match(term);
    const QString termName = regexMatch.captured(1);
    if (termName.isEmpty()) return matches;
    const QString termProject = regexMatch.captured(2);

    for (auto const &app: qAsConst(installed)) {
        if (app->nameArray[0].startsWith(termName, Qt::CaseInsensitive) ||
            (!app->nameArray[1].isEmpty() && app->nameArray[1].startsWith(termName, Qt::CaseInsensitive))) {
            for (int i = 0; i < app->recentlyUsed.size(); ++i) {
                const auto &dir = app->recentlyUsed.at(i);
                const QString dirName = dir.split(sep).last();
                if (termProject.isEmpty() || dirName.startsWith(termProject, Qt::CaseInsensitive)) {
                    Plasma::QueryMatch match(this);
                    match.setText(app->formatOptionText(formatString, dirName, dir));
                    match.setIconName(app->iconPath);
                    match.setData(QString(app->executablePath + dir));
                    match.setRelevance((float) 1 / (float) (i + 1));
                    if (displayInCategories) match.setMatchCategory(app->name);
                    matches.append(match);
                }
            }
        }
    }

    return matches;
}

QList<Plasma::QueryMatch> JetbrainsRunner::addProjectNameMatches(const QString &term) {
    QList<Plasma::QueryMatch> matches;
    for (auto const &app: qAsConst(installed)) {
        // If the plugin displays search suggestions by appname and the application name matches the search
        // term the options have already been created in the addAppNameMatches method
        // => this app should be skipped to avoid duplicates
        if ((launchByAppName &&
             app->nameArray[0].startsWith(term, Qt::CaseInsensitive)) ||
            (!app->nameArray[1].isEmpty() && app->nameArray[1].startsWith(term, Qt::CaseInsensitive))) {
            continue;
        }
        for (const auto &dir: qAsConst(app->recentlyUsed)) {
            const QString dirName = dir.split('/').last();
            if (dirName.startsWith(term, Qt::CaseInsensitive)) {
                Plasma::QueryMatch match(this);
                match.setText(app->formatOptionText(formatString, dirName, dir));
                match.setIconName(app->iconPath);
                match.setData(QString(app->executablePath + dir));
                if (displayInCategories) match.setMatchCategory(app->name);
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
                    if (githubRelease.value(QLatin1String("tag_name")).toString() > "1.3.1") {
                        displayText.append(githubRelease.value(QLatin1String("tag_name")).toString() + ": " +
                                           githubRelease.value(QLatin1String("name")).toString() + "\n");
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

QMimeData *JetbrainsRunner::mimeDataForMatch(const Plasma::QueryMatch &match)
{
    auto *data = new QMimeData();
    static const QString folderPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
        + sep
        + QStringLiteral("JetbrainsRunner")
        + sep;
    const QString desktopFileName =  folderPath
        + QString::fromLocal8Bit(QUrl::toPercentEncoding(match.text()))
        + QStringLiteral(".desktop");
    QDir folder(folderPath);
    if (!folder.exists()) {
        folder.mkpath(QStringLiteral("."));
    }
    if (!QFile::exists(desktopFileName)) {
        writeDesktopFile(match, desktopFileName);
    } else {
        KConfigGroup grp = KSharedConfig::openConfig(desktopFileName)->group("Desktop Entry");
         if (grp.readEntry("Exec") != match.data().toString()) {
             writeDesktopFile(match, desktopFileName);
         }
    }

    data->setUrls({QUrl::fromLocalFile(desktopFileName)});
    return data;
}
void JetbrainsRunner::writeDesktopFile(const Plasma::QueryMatch &match, const QString &filePath)
{
    QFile f(filePath);
    f.open(QFile::WriteOnly);
    f.write(QStringLiteral("[Desktop Entry]\n"
                           "Categories=Utility;Development;\n"
                           "Exec=%1\n"
                           "Icon=%2\n"
                           "Name=%3\n"
                           "Terminal=false\n"
                           "Type=Application\n")
                .arg(match.data().toString(), match.iconName(), match.text()).toLocal8Bit());
}

K_EXPORT_PLASMA_RUNNER(jetbrainsrunner, JetbrainsRunner)

// needed for the QObject subclass declared as part of K_EXPORT_PLASMA_RUNNER
#include "jetbrainsrunner.moc"
