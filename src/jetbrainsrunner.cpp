#include "jetbrainsrunner.h"

// KF
#include <KLocalizedString>
#include <KSharedConfig>
#include <QtGui/QtGui>

JetbrainsRunner::JetbrainsRunner(QObject *parent, const QVariantList &args)
        : Plasma::AbstractRunner(parent, args) {
    setObjectName(QStringLiteral("JetbrainsRunner"));
}

#include "JetbrainsApplication.h"
#include "SettingsDirectory.h"

JetbrainsRunner::~JetbrainsRunner() {
}

void JetbrainsRunner::init() {

    installed = JetbrainsApplication::getInstalledList();
    QList<SettingsDirectory> dirs = SettingsDirectory::getSettingsDirectories();
    SettingsDirectory::findCorrespondingDirectories(dirs, installed);
    JetbrainsApplication::parseXMLFiles(installed);
    installed = JetbrainsApplication::filterApps(installed);

    config = KSharedConfig::openConfig("krunnerrc")->group("Runners").group("JetBrainsRunner");

#ifdef LOG_INSTALLED
    for (const auto &i:installed) {
        qInfo() << "\n<------------ " << i.name << " ------------------->";
        for (const auto &d:i.recentlyUsed) {
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
    for (auto const &app:installed) {
        QRegExp regExp(R"(^(\w+)(?: (.+))?$)");
        regExp.indexIn(term);
        QString termName = regExp.capturedTexts().at(1);
        QString termProject = regExp.capturedTexts().last();
        if (QString(app.name).replace(" ", "").toLower().startsWith(termName)) {
            for (const auto &dir:app.recentlyUsed) {
                if (termProject.isEmpty() || dir.split('/').last().startsWith(termProject, Qt::CaseInsensitive)) {
                    Plasma::QueryMatch match(this);
                    match.setText(app.name + " launch " + dir.split('/').last());
                    match.setIconName(app.iconPath);
                    match.setData(QString(app.executablePath).replace("%f", dir));
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
        for (const auto &dir:app.recentlyUsed) {
            if (QString(dir).split('/').last().toLower().startsWith(term)) {
                Plasma::QueryMatch match(this);
                match.setText(" Launch " + dir.split('/').last() + " in " + app.name);
                match.setIconName(app.iconPath);
                match.setData(QString(app.executablePath).replace("%f", dir));
                matches.append(match);
            }
        }
    }
    return matches;
}

K_EXPORT_PLASMA_RUNNER(jetbrainsrunner, JetbrainsRunner)

// needed for the QObject subclass declared as part of K_EXPORT_PLASMA_RUNNER
#include "jetbrainsrunner.moc"
