#include <utility>
#include <QtCore>
#include "SettingsDirectory.h"
#include "JetbrainsApplication.h"


SettingsDirectory::SettingsDirectory(QString directory, QString name, QString version) : directory(std::move(
        directory)), name(std::move(name)), version(std::move(version)) {}

QList<SettingsDirectory> SettingsDirectory::getSettingsDirectories() {
    QString home = QDir::homePath();
    QList<SettingsDirectory> dirs;

    QDir directory(home);
    QStringList entries = directory.entryList(QDir::Hidden | QDir::Dirs);

    // Iterate reversed over entries
    int maxIndex = entries.size() - 1;
    for (int i = maxIndex; i <= maxIndex && i >= 0; i--) {
        auto const &e = entries.at(i);
        // Contains name and version number live testing => https://regex101.com/r/pMOkox/1
        if (e.contains(QRegExp(R"(^\.[A-Z][a-zA-Z]+(\d+\.\d+)$)"))) {
            QRegExp exp(R"(^\.([A-Z][a-zA-Z]+)(\d+\.\d+)$)");
            exp.indexIn(e);
            if (!exp.capturedTexts().empty()) {
                auto settingsDirectory = SettingsDirectory(home + "/" + e, exp.capturedTexts().at(1), exp.capturedTexts().last());
                dirs.append(settingsDirectory);
            }
        }
    }
    return dirs;
}

void SettingsDirectory::findCorrespondingDirectory(const QList<SettingsDirectory> &dirs, JetbrainsApplication* app) {

    app->name.remove(" Release");
    app->name.remove(" Edition");
    // Exact match or space in name
    for (const auto &dir :dirs) {
        if (dir.name == app->name) {
            app->configFolder = dir.directory + "/config/options/";
            return;
        }
        if (dir.name == QString(app->name).replace(" ", "")) {
            app->configFolder = dir.directory + "/config/options/";
            return;
        }
        if (dir.name == app->name.remove(" RC").remove(" EAP")) {
            app->configFolder = dir.directory + "/config/options/";
            return;
        }
    }

    // Handle Ultimate/Community editions and experimental java runtime
    QMap<QString, QString> aliases = getAliases();
    if (aliases.count(app->name.remove(" + JBR11")) == 0) return;
    for (const auto &dir :dirs) {
        if (dir.name == aliases.find(app->name).value()) {
            app->configFolder = dir.directory + "/config/options/";
            return;
        }
    }
}


QMap<QString, QString> SettingsDirectory::getAliases() {
    return {
            {"IntelliJ IDEA Community",   "IdeaIC"},
            {"IntelliJ IDEA Ultimate",    "IntelliJIdea"},
            {"PyCharm Professional",      "PyCharm"},
            {"PyCharm Community",         "PyCharmCE"}
    };
}

void SettingsDirectory::findCorrespondingDirectories(const QList<SettingsDirectory> &dirs,
                                                     QList<JetbrainsApplication*> &apps) {
    for (auto &app:apps) {
        findCorrespondingDirectory(dirs, app);
    }

}
