#include "JetbrainsApplication.h"
#include <KSharedConfig>
#include <KConfigCore/KConfigGroup>
#include <QtGui/QtGui>

JetbrainsApplication::JetbrainsApplication(const QString &desktopFilePath) : desktopFilePath(desktopFilePath) {
    KConfigGroup config = KSharedConfig::openConfig(desktopFilePath)->group("Desktop Entry");
    iconPath = config.readEntry("Icon");
    executablePath = config.readEntry("Exec");
    name = config.readEntry("Name");
}

QList<JetbrainsApplication> JetbrainsApplication::getInstalledList() {
    QList<JetbrainsApplication> installed;
    QString home = QDir::homePath();

    // Manually, locally or with Toolbox installed
    QProcess toolBoxProcess;
    toolBoxProcess.start("sh", QStringList() << "-c" << "ls ~/.local/share/applications | grep jetbrains");
    toolBoxProcess.waitForFinished();
    for (const auto &item :toolBoxProcess.readAllStandardOutput().split('\n')) {
        if (!item.isEmpty()) {
            if (item == "JetBrains Toolbox") continue;
            installed.append(JetbrainsApplication(home + "/.local/share/applications/" + item));
        }
    }
    // Using snap installed
    QProcess snapProcess;
    snapProcess.start("sh", QStringList() << "-c" << "grep -rl '/var/lib/snapd/desktop/applications/' -e 'jetbrains'");
    snapProcess.waitForFinished();
    for (const auto &item :snapProcess.readAllStandardOutput().split('\n')) {
        if (!item.isEmpty()) {
            installed.append(JetbrainsApplication(item));
        }
    }
    //Globally, manually installed
    QProcess globallyInstalledProcess;
    globallyInstalledProcess.start("sh", QStringList() << "-c" << "ls /usr/share/applications | grep jetbrains");
    globallyInstalledProcess.waitForFinished();
    for (const auto &item :globallyInstalledProcess.readAllStandardOutput().split('\n')) {
        if (!item.isEmpty()) {
            installed.append(JetbrainsApplication("/usr/share/applications/" + item));
        }
    }
    return installed;
}

void JetbrainsApplication::parseXMLFile() {
    // Recent folders are in recentProjectDirectories.xml or in recentProjects.xml located
    QString content = "";
    if (this->configFolder.isEmpty()) return;
    QFile f(this->configFolder + "recentProjectDirectories.xml");
    if (!f.exists()) {
        QFile f2(this->configFolder + "recentProjects.xml");
        if (!f2.open(QIODevice::ReadOnly)) {
            f2.close();
            //qInfo() << "No entry found for " << this->name;
            return;
        }
        content = f2.readAll();
        f2.close();
    } else {
        f.open(QIODevice::ReadOnly);
        content = f.readAll();
        f.close();
    }

    // Extract paths from XML file content
    QXmlStreamReader reader(content);
    while (reader.readNextStartElement()) {
        if (reader.name() == "option" && reader.attributes().value("name") == "recentPaths") {
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
    }
}

void JetbrainsApplication::parseXMLFiles(QList<JetbrainsApplication> &apps) {
    for (auto &app:apps) {
        app.parseXMLFile();
    }
}

QList<JetbrainsApplication> JetbrainsApplication::filterApps(QList<JetbrainsApplication> &apps) {
    QList<JetbrainsApplication> notEmpty;
    for (auto const &app:apps) {
        if (!app.recentlyUsed.empty()) {
            notEmpty.append(app);
        }
#ifdef LOG_INSTALLED
        else {
            qInfo() << "Found not projects for: " << app.name;
        }
#endif
    }
    return notEmpty;
}
