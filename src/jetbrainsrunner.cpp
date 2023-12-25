#include "jetbrainsrunner.h"
#include "jetbrains-api/JetbrainsApplication.h"
#include "jetbrains-api/ConfigKeys.h"

#include <krunner_version.h>
#include <KIO/CommandLauncherJob>
#include <KIO/DesktopExecParser>
#include <KLocalizedString>
#include <KService>
#include <KSharedConfig>
#include <KShell>
#include <QDate>
#include <QStringBuilder>
#include <QLoggingCategory>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMimeData>
#include <QProcess>

#include "jetbrains-api/export.h"

// Generated in static lib
Q_DECLARE_LOGGING_CATEGORY(JETBRAINS)


JetbrainsRunner::~JetbrainsRunner() {
    qDeleteAll(installed);
}

void JetbrainsRunner::init() {
    const QString configFolder = QDir::homePath() + QStringLiteral("/.config/krunnerplugins/");
    const QDir configDir(configFolder);
    if (!configDir.exists()) {
        configDir.mkpath(configFolder);
    }
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
    KConfigGroup config = KSharedConfig::openConfig(QStringLiteral("krunnerplugins/jetbrainsrunnerrc"))
        ->group("Config");
    if (!configFile.isEmpty()) {
        config.config()->reparseConfiguration();
    }

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
    if (!formatString.contains(QLatin1String(FormatString::DIR)) &&
        !formatString.contains(QLatin1String(FormatString::PROJECT))) {
        formatString = Config::formatStringDefault;
    }
    launchByAppName = config.readEntry(Config::launchByAppName, true);
    launchByProjectName = config.readEntry(Config::launchByProjectName, true);
    displayInCategories = config.readEntry(Config::displayInCategories, false);
    searchResultChoice = (SearchResultChoice) config.readEntry(Config::filterSearchResults, (int) SearchResultChoice::ProjectNameStartsWith);

    qDeleteAll(installed);
    installed.clear();
    if (launchByAppName) {
        appNameRegex = QRegularExpression(QStringLiteral(R"(^(\w+)(?: (.+))?$)"));
        appNameRegex.optimize();
    }

    installed = JetbrainsAPI::fetchApplications(config);

    // Version update notification
    QDate lastUpdateCheckDate = QDate::fromString(config.readEntry(Config::checkedUpdateDate));
    if (config.readEntry(Config::notifyUpdates, true) && (
        !lastUpdateCheckDate.isValid() ||
        lastUpdateCheckDate.addDays(7) <= QDate::currentDate())) {
        auto manager = new QNetworkAccessManager(this);
        QNetworkRequest request(
            QUrl(QStringLiteral("https://api.github.com/repos/alex1701c/JetBrainsRunner/releases")));
        manager->get(request);
        connect(manager, &QNetworkAccessManager::finished, this, &JetbrainsRunner::displayUpdateNotification);
        config.writeEntry(Config::checkedUpdateDate, QDate::currentDate().toString());
    }

    QList<KRunner::RunnerSyntax> syntaxes;
    const QLatin1String projectPlaceholder("<project name>");
    if (launchByAppName) {
        QStringList exampleQueries{QLatin1String("<app name> ") + projectPlaceholder};
        for (const auto app : std::as_const(installed)) {
            exampleQueries << app->name.split(QLatin1Char(' ')).constFirst().toLower() + QLatin1Char(' ') + projectPlaceholder;
        }
#if KRUNNER_VERSION < QT_VERSION_CHECK(5, 106, 0)
        KRunner::RunnerSyntax syn(exampleQueries.takeFirst(), QStringLiteral("Searches the projects of the given jetbrains app"));
        for (const QString &str: exampleQueries) {
            syn.addExampleQuery(str);
        }
#else
        syntaxes.append(KRunner::RunnerSyntax{exampleQueries, QStringLiteral("Searches the projects of the given jetbrains app")});
#endif
    }
    if (launchByAppName) {
        syntaxes.append(KRunner::RunnerSyntax{projectPlaceholder, QStringLiteral("Search for projects directly without typing the app name")});
    }
    setSyntaxes(syntaxes);
}

void JetbrainsRunner::match(KRunner::RunnerContext &context) {
    const QString term = context.query().toLower();
    if (launchByAppName) {
        context.addMatches(addAppNameMatches(term));
    }
    if (launchByProjectName) {
        context.addMatches(addProjectNameMatches(term));
    }
    context.addMatches(addPathNameMatches(context.query()));
}

void JetbrainsRunner::run(const KRunner::RunnerContext & /*context*/, const KRunner::QueryMatch &match)
{
    const auto cmdAndProject = match.data().toStringList();
    const QString command = cmdAndProject.at(0);
    const QString project = cmdAndProject.at(1);

    const KService service(match.matchCategory(), command, QString());
    KIO::DesktopExecParser parser(service, QList{QUrl::fromLocalFile(project)});
    QStringList args = parser.resultingArguments();

    qDebug(JETBRAINS).noquote() << "starting IDE" << args;
    auto *job = new KIO::CommandLauncherJob(args.takeFirst(), args);
    job->start();
}

QList<KRunner::QueryMatch> JetbrainsRunner::addAppNameMatches(const QString &term) {
    QList<KRunner::QueryMatch> matches;
    if (term.size() < 2) {
        return matches;
    }

    const auto regexMatch = appNameRegex.match(term);
    const QString termName = regexMatch.captured(1);
    if (termName.isEmpty()) return matches;
    const QString termProject = regexMatch.captured(2);

    for (auto const &app: qAsConst(installed)) {
        if (app->nameArray[0].startsWith(termName, Qt::CaseInsensitive) ||
            (!app->nameArray[1].isEmpty() && app->nameArray[1].startsWith(termName, Qt::CaseInsensitive))) {
            for (int i = 0; i < app->recentlyUsed.size(); ++i) {
                const auto &project = app->recentlyUsed.at(i);
                if (termProject.isEmpty() || projectMatchesQuery(termProject, project)) {
                    KRunner::QueryMatch match(this);
                    match.setText(app->formatOptionText(formatString, project));
                    match.setIconName(app->iconPath);
                    match.setData(QStringList{app->executablePath, project.path});
                    match.setRelevance((float) 1 / (float) (i + 1));
                    if (displayInCategories) match.setMatchCategory(app->name);
                    matches.append(match);
                }
            }
        }
    }

    return matches;
}

QList<KRunner::QueryMatch> JetbrainsRunner::addProjectNameMatches(const QString &term) {
    QList<KRunner::QueryMatch> matches;
    if (term.size() < 3) {
        return matches;
    }
    for (auto const &app: qAsConst(installed)) {
        // If the plugin displays search suggestions by appname and the application name matches the search
        // term the options have already been created in the addAppNameMatches method
        // => this app should be skipped to avoid duplicates
        if ((launchByAppName &&
             app->nameArray[0].startsWith(term, Qt::CaseInsensitive)) ||
            (!app->nameArray[1].isEmpty() && app->nameArray[1].startsWith(term, Qt::CaseInsensitive))) {
            continue;
        }
        for (const auto &project: qAsConst(app->recentlyUsed)) {
            if (projectMatchesQuery(term, project)) {
                KRunner::QueryMatch match(this);
                match.setText(app->formatOptionText(formatString, project));
                match.setIconName(app->iconPath);
                match.setData(QStringList{app->executablePath, project.path});
                if (displayInCategories) match.setMatchCategory(app->name);
                matches.append(match);
            }
        }
    }
    return matches;
}

bool JetbrainsRunner::projectMatchesQuery(const QString &term, const Project &project) const {
    if (searchResultChoice == SearchResultChoice::ProjectNameContains) {
        return project.name.contains(term, Qt::CaseInsensitive);
    } else if (searchResultChoice == SearchResultChoice::PathContains) {
        return project.path.contains(term, Qt::CaseInsensitive);
    } else if (searchResultChoice == SearchResultChoice::ProjectNameStartsWith){
        return project.name.startsWith(term, Qt::CaseInsensitive);
    }
    qCritical() << "Found no value for" << searchResultChoice;
    return project.name.startsWith(term, Qt::CaseInsensitive);
}

void JetbrainsRunner::displayUpdateNotification(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QString displayText;
        const auto jsonObject = QJsonDocument::fromJson(reply->readAll());
        if (jsonObject.isArray()) {
            const auto array = jsonObject.array();
            for (const auto &githubReleaseObj:array) {
                if (githubReleaseObj.isObject()) {
                    const auto githubRelease = githubReleaseObj.toObject();
                    const QString tagName = githubRelease.value(QLatin1String("tag_name")).toString();
                    if (tagName > QLatin1String(CMAKE_PROJECT_VERSION)) {
                        const QString name = githubRelease.value(QLatin1String("name")).toString();
                        displayText.append(tagName+ ": " + name + "\n");
                    }
                }
            }
        }
        if (!displayText.isEmpty()) {
            displayText.prepend("New Versions Available:\n");
            displayText.append("Please go to https://github.com/alex1701c/JetBrainsRunner</a>");
            QProcess::startDetached(QStringLiteral("notify-send"), {
                "JetBrains Runner Updates!", displayText, "--icon", "jetbrains", "--expire-time", "5000"
            });
        }
    }
}

QMimeData *JetbrainsRunner::mimeDataForMatch(const KRunner::QueryMatch &match) {
    auto *data = new QMimeData();
    static const QString folderPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                                      + sep
                                      + QStringLiteral("JetbrainsRunner")
                                      + sep;
    const QString desktopFileName = folderPath
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

void JetbrainsRunner::writeDesktopFile(const KRunner::QueryMatch &match, const QString &filePath) {
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

QList<KRunner::QueryMatch> JetbrainsRunner::addPathNameMatches(const QString &term) {
    const auto regexMatch = appNameRegex.match(term);
    if (!regexMatch.hasMatch()) {
        return {};
    }
    const QString termName = regexMatch.captured(1);
    const QString termProject = regexMatch.captured(2).replace('~', QDir::homePath());
    QList<KRunner::QueryMatch> matches;
    if (!termProject.isEmpty() && QFileInfo(termProject).isDir()) {
        for (auto const &app: qAsConst(installed)) {
            if (app->nameArray[0].startsWith(termName, Qt::CaseInsensitive) ||
                (!app->nameArray[1].isEmpty() && app->nameArray[1].startsWith(termName, Qt::CaseInsensitive))) {
                KRunner::QueryMatch match(this);
                match.setText(QStringLiteral("Open %1 in %2").arg(regexMatch.captured(2), app->name));
                match.setIconName(app->iconPath);
                match.setData(QStringList{app->executablePath, termProject});
                matches.append(match);
            }
        }
    }
    return matches;
}

K_PLUGIN_CLASS_WITH_JSON(JetbrainsRunner, "jetbrainsrunner.json")

// needed for the QObject subclass declared as part of K_EXPORT_PLASMA_RUNNER
#include "jetbrainsrunner.moc"
#include "moc_jetbrainsrunner.cpp"
