//
// Created by alex on 24.06.19.
//

#ifndef JETBRAINSRUNNER_SETTINGSDIRECTORY_H
#define JETBRAINSRUNNER_SETTINGSDIRECTORY_H


#include "JetbrainsApplication.h"

class SettingsDirectory {
public:

    static QMap<QString, QString> getAliases();

    SettingsDirectory(const QString &directory, const QString &name, const QString &version);

    static QList<SettingsDirectory> getSettingsDirectories();

    static void findCorrespoindingDirectory(QList<SettingsDirectory> dirs, JetbrainsApplication app);

    QString directory;
    QString name;
    QString version;
};


#endif //JETBRAINSRUNNER_SETTINGSDIRECTORY_H
