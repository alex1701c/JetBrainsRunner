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

QList<JetbrainsApplication *> JetbrainsApplication::getInstalledList() {
    QList<JetbrainsApplication *> installed;
    QString home = QDir::homePath();

    // Manually, locally or with Toolbox installed
    QProcess toolBoxProcess;
    toolBoxProcess.start("sh", QStringList() << "-c" << "ls ~/.local/share/applications | grep jetbrains");
    toolBoxProcess.waitForFinished();
    for (const auto &item :toolBoxProcess.readAllStandardOutput().split('\n')) {
        if (!item.isEmpty()) {
            if (item == "JetBrains Toolbox") continue;
            installed.append(new JetbrainsApplication(home + "/.local/share/applications/" + item));
        }
    }
    // Using snap installed
    QProcess snapProcess;
    snapProcess.start("sh", QStringList() << "-c" << "grep -rl '/var/lib/snapd/desktop/applications/' -e 'jetbrains'");
    snapProcess.waitForFinished();
    for (const auto &item :snapProcess.readAllStandardOutput().split('\n')) {
        if (!item.isEmpty()) {
            installed.append(new JetbrainsApplication(item));
        }
    }
    //Globally, manually installed
    QProcess globallyInstalledProcess;
    globallyInstalledProcess.start("sh", QStringList() << "-c" << "ls /usr/share/applications | grep jetbrains");
    globallyInstalledProcess.waitForFinished();
    for (const auto &item :globallyInstalledProcess.readAllStandardOutput().split('\n')) {
        if (!item.isEmpty()) {
            installed.append(new JetbrainsApplication("/usr/share/applications/" + item));
        }
    }
    for (const auto &aurInstalledFile:getAURInstalledFiles()) {
        installed.append(new JetbrainsApplication(aurInstalledFile));
    }
    return installed;
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

QStringList JetbrainsApplication::getAURInstalledFiles() {
    QStringList aurFiles = {
            "rubymine.desktop",
            "pycharm-professional.desktop",
            "pycharm-eap.desktop",
            "charm.desktop",
            "jetbrains-clion.desktop",
            "rider.desktop",
    };
    const QString installationLocation = "/usr/share/applications/";
    QStringList validAurFiles;
    for (const auto &aurFile:aurFiles) {
        if (QFile::exists(installationLocation + aurFile)) validAurFiles.append(installationLocation + aurFile);
    }

    return validAurFiles;
}

