//
// Created by alex on 24.06.19.
//

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

void JetbrainsApplication::parseXMLFile() {
    QString content = "";
    QFile f(this->configFolder + "recentProjectDirectories.xml");
    if (!f.exists()) {
        QFile f2(this->configFolder + "recentProjects.xml");
        if (!f2.open(QIODevice::ReadOnly)) {
            f2.close();
            qInfo() << "Nothing found for " << this->name;
            return;
        }
        content = f2.readAll();
        f2.close();

    } else {
        f.open(QIODevice::ReadOnly);
        content = f.readAll();
        f.close();
    }
    QXmlStreamReader reader(content);
    while (reader.readNextStartElement()) {
        if (reader.name() == "option" && reader.attributes().value("name") == "recentPaths") {
            while (reader.readNextStartElement()) {
                if (reader.name() == "option") {
                    this->recentlyUsed.append(
                            reader.attributes().value("value").toString().replace("$USER_HOME$", QDir::homePath())
                    );
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

