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
    connect(this, SIGNAL(fileChanged(QString)), this, SLOT(configChanged(QString)));
}


void JetbrainsApplication::parseXMLFile(QString content) {
    // Recent folders are in recentProjectDirectories.xml or in recentProjects.xml located
    // If the method is triggered by the file watcher the content is provided
    if (content.isEmpty()) {
        if (this->configFolder.isEmpty()) return;
        QFile f(this->configFolder + "recentProjectDirectories.xml");
        if (!f.exists()) {
            QFile f2(this->configFolder + "recentProjects.xml");
            if (!f2.open(QIODevice::ReadOnly)) {
                f2.close();
#ifdef LOG_INSTALLED
                qInfo() << "No entry found for " << this->name;
#endif
                return;
            }
            this->addPath(f2.fileName());
            content = f2.readAll();
            f2.close();
        } else {
            if (f.open(QIODevice::ReadOnly)) {
                content = f.readAll();
                f.close();
                this->addPath(f.fileName());
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
}

void JetbrainsApplication::parseXMLFiles(QList<JetbrainsApplication *> &apps) {
    for (auto &app:apps) {
        app->parseXMLFile();
    }
}

QList<JetbrainsApplication *> JetbrainsApplication::filterApps(QList<JetbrainsApplication *> &apps) {
    QList<JetbrainsApplication *> notEmpty;
    for (auto const &app:apps) {
        if (!app->recentlyUsed.empty()) {
            notEmpty.append(app);
        }
#ifdef LOG_INSTALLED
        else {
            qInfo() << "Found not projects for: " << app->name;
        }
#endif
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
    for (const auto &aurFile:additionalDesktopFileLocations) {
        if (QFile::exists(aurFile)) validFiles.append(aurFile);
    }

    return validFiles;
}

QMap<QString, QString> JetbrainsApplication::getInstalledApplicationPaths(const KConfigGroup &customMappingConfig) {
    QMap<QString, QString> applicationPaths;
    const QString home = QDir::homePath();


    // Manually, locally or with Toolbox installed
    const QString localPath = home + "/.local/share/applications/";
    QDir localJetbrainsApplications(localPath);
    localJetbrainsApplications.setNameFilters(QStringList{"jetbrains-*"});
    for (const auto &item : localJetbrainsApplications.entryList()) {
        if (!item.isEmpty()) {
            applicationPaths.insert(filterApplicationName(KSharedConfig::openConfig(localPath + item)->
                    group("Desktop Entry").readEntry("Name")), localPath + item);
        }
    }
    // Globally installed
    const QString globalPath = "/usr/share/applications/";
    QDir globalJetbrainsApplications(globalPath);
    globalJetbrainsApplications.setNameFilters(QStringList{"jetbrains-*"});
    for (const auto &item : globalJetbrainsApplications.entryList()) {
        if (!item.isEmpty()) {
            applicationPaths.insert(filterApplicationName(KSharedConfig::openConfig(globalPath + item)->
                    group("Desktop Entry").readEntry("Name")), globalPath + item);
        }
    }

    // AUR/Snap/Other  installed
    for (const auto &item : getAdditionalDesktopFileLocations()) {
        if (!item.isEmpty()) {
            applicationPaths.insert(filterApplicationName(KSharedConfig::openConfig(item)->
                    group("Desktop Entry").readEntry("Name")), item);
        }
    }

    // Add manually configured entries
    for (const auto &mappingEntry: customMappingConfig.entryMap().toStdMap()) {
        if (QFile::exists(mappingEntry.first) && QFile::exists(mappingEntry.second)) {
            applicationPaths.insert(filterApplicationName(KSharedConfig::openConfig(mappingEntry.first)->
                    group("Desktop Entry").readEntry("Name")), mappingEntry.first);
        }
    }

    return applicationPaths;
}

QString JetbrainsApplication::filterApplicationName(const QString &name) {
    return QString(name).remove(" Release").remove(" Edition").remove(" + JBR11").remove(" RC").remove(" EAP");
}

