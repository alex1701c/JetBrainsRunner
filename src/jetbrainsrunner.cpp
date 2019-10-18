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

    installed = JetbrainsApplication::getInstalledList();
#ifdef LOG_INSTALLED
    qInfo() << "\n<-------- Read installed Jetbrains Applications ------------>";
    for (const auto &app:installed) {
        qInfo() << app->name;
    }
#endif
    QList<SettingsDirectory> dirs = SettingsDirectory::getSettingsDirectories();
#ifdef LOG_INSTALLED
    qInfo() << "\n<-------- All possible Settings directories ------------>";
    for (const auto &dir:dirs) {
        qInfo() << dir.directory << dir.name << dir.version;
    }
    qInfo() << "\n";
#endif
    SettingsDirectory::findCorrespondingDirectories(dirs, installed);
    JetbrainsApplication::parseXMLFiles(installed);
    installed = JetbrainsApplication::filterApps(installed);

    config = KSharedConfig::openConfig("krunnerrc")->group("Runners").group("JetBrainsRunner");
    QDate lastUpdateCheckDate = QDate::fromString(config.readEntry("checkedUpdateDate"));
    if (!config.hasKey("checkedUpdateDate") || !lastUpdateCheckDate.isValid() ||
        lastUpdateCheckDate.addDays(7) <= QDate::currentDate()) {
        auto manager = new QNetworkAccessManager(this);
        QNetworkRequest request(QUrl("https://api.github.com/repos/alex1701c/JetBrainsRunner/releases"));
        manager->get(request);
        connect(manager, SIGNAL(finished(QNetworkReply * )), this, SLOT(displayUpdateNotification(QNetworkReply * )));
    }
#ifdef LOG_INSTALLED
    qInfo() << "<------------- Projects and their recently used project paths ----------------------->";
    for (const auto &i:installed) {
        qInfo() << "\n<------------ " << i->name << " ------------------->";
        for (const auto &d:i->recentlyUsed) {
            qInfo() << d;
        }
    }
#endif
}


void JetbrainsRunner::match(Plasma::RunnerContext &context) {
    const QString term = context.query().toLower();
    QList<Plasma::QueryMatch> matches;

    if (config.readEntry("LaunchByAppName", "true") == "true") {
        matches.append(addAppNameMatches(term));
    }
    if (config.readEntry("LaunchByProjectName", "true") == "true") {
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

    QRegExp regExp(R"(^(\w+)(?: (.+))?$)");
    regExp.indexIn(term);
    QString termName = regExp.capturedTexts().at(1);
    QString termProject = regExp.capturedTexts().last();
    if (termName.isEmpty()) return matches;

    for (auto const &app:installed) {
        if (QString(app->name).replace(" ", "").startsWith(termName, Qt::CaseInsensitive)) {
            const int recentProjectsCount = app->recentlyUsed.size();
            for (int i = 0; i < recentProjectsCount; ++i) {
                const auto &dir = app->recentlyUsed.at(i);
                if (termProject.isEmpty() || dir.split('/').last().startsWith(termProject, Qt::CaseInsensitive)) {
                    Plasma::QueryMatch match(this);
                    match.setText(app->name + " launch " + dir.split('/').last());
                    match.setIconName(app->iconPath);
                    match.setData(QString(app->executablePath).replace("%f", dir));
                    match.setRelevance((float) 1 / (i + 1));
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
