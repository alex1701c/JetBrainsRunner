#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-pass-by-value"
//
// Created by alex on 24.06.19.
//

#include <QtCore>
#include "SettingsDirectory.h"
#include "JetbrainsApplication.h"


SettingsDirectory::SettingsDirectory(const QString &directory, const QString &name, const QString &version) : directory(
        directory), name(name), version(version) {
}

QList<SettingsDirectory> SettingsDirectory::getSettingsDirectories() {
    QString home = QDir::homePath();
    QList<SettingsDirectory> dirs;

    QDir directory(home);
    QStringList entries = directory.entryList(QDir::Hidden | QDir::Dirs);

    // Iterate reversed over entries
    int midx = entries.size() - 1;
    for (int i = midx; i <= midx && i >= 0; i--) {
        auto const &e = entries.at(i);
        if (e.contains(QRegExp(R"(^\.[A-Z][a-zA-Z]+(\d+\.\d+)$)"))) {
            QRegExp exp(R"(^\.([A-Z][a-zA-Z]+)(\d+\.\d+)$)");
            exp.indexIn(e);
            if (!exp.capturedTexts().empty()) {
                auto sdir = SettingsDirectory(home + "/" + e, exp.capturedTexts().at(1), exp.capturedTexts().last());
                dirs.append(sdir);
            }
        }
    }
    return dirs;
}

void SettingsDirectory::findCorrespondingDirectory(const QList<SettingsDirectory> &dirs, JetbrainsApplication &app) {

    // Exact match or space in name
    for (const auto &dir :dirs) {
        if (dir.name == app.name) {
            qInfo() << app.desktopFilePath.split('/').last() << " --> " << dir.directory;
            return;
        }
        if (dir.name == QString(app.name).replace(" ", "")) {
            qInfo() << app.desktopFilePath.split('/').last() << " --> " << dir.directory;
            return;
        }
    }

    // Handle ultimate/community editions and experimental runtimes
    QMap<QString, QString> aliases = getAliases();
    if (aliases.count(app.name.replace(" + JBR11", "")) == 0)return;
    for (const auto &dir :dirs) {
        if (dir.name == aliases.find(app.name).value()) {
            qInfo() << app.desktopFilePath.split('/').last() << " ---alias--> " << dir.directory;
            return;
        }
    }
}


QMap<QString, QString> SettingsDirectory::getAliases() {
    return {
            {"IntelliJ IDEA Community",   "IdeaIC"},
            {"IntelliJ IDEA Ultimate",    "IntelliJIdea"},
            {"PyCharm Professional",      "PyCharm"},
            {"PyCharm Community Edition", "PyCharmCE"},
    };
}


#pragma clang diagnostic pop