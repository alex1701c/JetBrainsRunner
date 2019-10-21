#include "JetbrainsApplication.h"
#include <KSharedConfig>
#include <KConfigCore/KConfigGroup>
#include <QtGui/QtGui>

JetbrainsApplication::JetbrainsApplication(const QString &desktopFilePath) :
        QFileSystemWatcher(nullptr), desktopFilePath(desktopFilePath) {
    KConfigGroup config = KSharedConfig::openConfig(desktopFilePath)->group("Desktop Entry");
    iconPath = config.readEntry("Icon");
    executablePath = config.readEntry("Exec");
    name = config.readEntry("Name");

    // Allow the user to search for both names like Android Studio
    auto nameList = filterApplicationName(QString(name))
            .remove(" Professional")
            .remove(" Community")
            .remove(" Ultimate")
            .remove(" Edu")
            .split(" ");
    if (nameList.count() > 0) {
        nameArray[0] = nameList.at(0);
        if (nameList.count() == 2) {
            secondName = true;
            nameArray[1] = nameList.at(1);
        }

    }
    connect(this, SIGNAL(fileChanged(QString)), this, SLOT(configChanged(QString)));
}


void JetbrainsApplication::parseXMLFile(QString content, QString *debugMessage) {
    // Recent folders are in recentProjectDirectories.xml or in recentProjects.xml located
    // If the method is triggered by the file watcher the content is provided
    if (content.isEmpty()) {
        if (this->configFolder.isEmpty()) return;
        QFile f(this->configFolder + "recentProjectDirectories.xml");
        if (!f.exists()) {
            QFile f2(this->configFolder + "recentProjects.xml");
            if (!f2.open(QIODevice::ReadOnly)) {
                f2.close();
                if (debugMessage != nullptr) {
                    debugMessage->append("No config file found for " + this->name + " " + this->desktopFilePath + "\n");
                }
                return;
            }
            this->addPath(f2.fileName());
            if (debugMessage != nullptr) {
                debugMessage->append("Config file found for " + this->name + " " + f2.fileName() + "\n");
            }
            content = f2.readAll();
            f2.close();
        } else {
            if (f.open(QIODevice::ReadOnly)) {
                content = f.readAll();
                f.close();
                this->addPath(f.fileName());
                if (debugMessage != nullptr) {
                    debugMessage->append("Config file found for " + this->name + " " + f.fileName() + "\n");
                }
            }
        }
    }

    if (content.isEmpty()) return;

    // Go to RecentDirectoryProjectsManager component
    QXmlStreamReader reader(content);
    reader.readNextStartElement();
    reader.readNextStartElement();

    // Go through elements until the recentPaths option element is selected
    for (int i = 0; i < 4; ++i) {
        reader.readNextStartElement();
        if (reader.name() != "option" || reader.attributes().value("name") != "recentPaths") {
            reader.skipCurrentElement();
        } else {
            reader.name();
            break;
        }
    }

    // Extract paths from XML element
    while (reader.readNextStartElement()) {
        if (reader.name() == "option") {
            QString recentPath = reader.attributes().value("value").toString().replace("$USER_HOME$", QDir::homePath());
            if (QDir(recentPath).exists()) {
                this->recentlyUsed.append(recentPath);
            }
            reader.readElementText();
        }
    }
    if (debugMessage != nullptr) {
        for (const auto &recent:recentlyUsed) {
            debugMessage->append("Recently used project folder for " + this->name + " " + recent + "\n");
        }
    }
}

void JetbrainsApplication::parseXMLFiles(QList<JetbrainsApplication *> &apps, QString *debugMessage) {
    for (auto &app:apps) {
        app->parseXMLFile("", debugMessage);
    }
}

QList<JetbrainsApplication *> JetbrainsApplication::filterApps(QList<JetbrainsApplication *> &apps, QString *debugMessage) {
    QList<JetbrainsApplication *> notEmpty;
    if (debugMessage != nullptr) {
        debugMessage->append("========== Filter Jetbrains Apps ==========\n");
    }
    for (auto const &app:apps) {
        if (!app->recentlyUsed.empty()) {
            notEmpty.append(app);
        } else if (debugMessage != nullptr) {
            debugMessage->append("Found not projects for: " + app->name + "\n");
        }
    }
    return notEmpty;
}

QStringList JetbrainsApplication::getAdditionalDesktopFileLocations() {
    QStringList additionalDesktopFileLocations = {
            "/usr/share/applications/rubymine.desktop",
            "/usr/share/applications/pycharm-professional.desktop",
            "/usr/share/applications/pycharm-eap.desktop",
            "/usr/share/applications/charm.desktop",
            "/usr/share/applications/rider.desktop",
    };
    QStringList validFiles;
    for (const auto &additionalFile:additionalDesktopFileLocations) {
        if (QFile::exists(additionalFile)) validFiles.append(additionalFile);
    }

    return validFiles;
}

QMap<QString, QString>
JetbrainsApplication::getInstalledApplicationPaths(const KConfigGroup &customMappingConfig, QString *debugMessage) {
    QMap<QString, QString> applicationPaths;
    const QString home = QDir::homePath();


    // Manually, locally or with Toolbox installed
    const QString localPath = home + "/.local/share/applications/";
    QDir localJetbrainsApplications(localPath);
    localJetbrainsApplications.setNameFilters(QStringList{"jetbrains-*"});
    if (debugMessage != nullptr) {
        debugMessage->append("========== Locally Installed Jetbrains Applications ==========\n");
    }
    for (const auto &item : localJetbrainsApplications.entryList()) {
        if (!item.isEmpty()) {
            applicationPaths.insert(filterApplicationName(KSharedConfig::openConfig(localPath + item)->
                    group("Desktop Entry").readEntry("Name")), localPath + item);
            if (debugMessage != nullptr) {
                debugMessage->append(localPath + item + "\n");
            }
        }
    }
    // Globally installed
    const QString globalPath = "/usr/share/applications/";
    QDir globalJetbrainsApplications(globalPath);
    globalJetbrainsApplications.setNameFilters(QStringList{"jetbrains-*"});
    if (debugMessage != nullptr) {
        debugMessage->append("========== Globally Installed Jetbrains Applications ==========\n");
    }
    for (const auto &item : globalJetbrainsApplications.entryList()) {
        if (!item.isEmpty()) {
            applicationPaths.insert(filterApplicationName(KSharedConfig::openConfig(globalPath + item)->
                    group("Desktop Entry").readEntry("Name")), globalPath + item);
            if (debugMessage != nullptr) {
                debugMessage->append(globalPath + item + "\n");
            }
        }
    }

    // AUR/Snap/Other  installed
    if (debugMessage != nullptr) {
        debugMessage->append("========== AUR/Snap/Other Installed Jetbrains Applications ==========\n");
    }
    for (const auto &item : getAdditionalDesktopFileLocations()) {
        if (!item.isEmpty()) {
            applicationPaths.insert(filterApplicationName(KSharedConfig::openConfig(item)->
                    group("Desktop Entry").readEntry("Name")), item);
            if (debugMessage != nullptr) {
                debugMessage->append(item + "\n");
            }
        }
    }

    // Add manually configured entries
    if (debugMessage != nullptr) {
        debugMessage->append("========== Manually Configured Jetbrains Applications ==========\n");
    }
    for (const auto &mappingEntry: customMappingConfig.entryMap().toStdMap()) {
        if (QFile::exists(mappingEntry.first) && QFile::exists(mappingEntry.second)) {
            applicationPaths.insert(filterApplicationName(KSharedConfig::openConfig(mappingEntry.first)->
                    group("Desktop Entry").readEntry("Name")), mappingEntry.first);
            if (debugMessage != nullptr) {
                debugMessage->append(mappingEntry.first + "\n");
                debugMessage->append(mappingEntry.second + "\n");
            }
        }
    }

    return applicationPaths;
}

QString JetbrainsApplication::filterApplicationName(const QString &name) {
    return QString(name).remove(" Release").remove(" Edition").remove(" + JBR11").remove(" RC").remove(" EAP");
}
