//
// Created by alex on 24.06.19.
//

#ifndef JETBRAINSRUNNER_SETTINGSDIRECTORY_H
#define JETBRAINSRUNNER_SETTINGSDIRECTORY_H


#include "JetbrainsApplication.h"

class SettingsDirectory {
public:

    static QMap<QString, QString> getAliases();

    SettingsDirectory(QString directory, QString name, QString version);

    static QList<SettingsDirectory> getSettingsDirectories();

    static void findCorrespondingDirectory(const QList<SettingsDirectory> &dirs, JetbrainsApplication &app);
    static void findCorrespondingDirectories(const QList<SettingsDirectory> &dirs, QList<JetbrainsApplication> &apps);


    QString directory;
    QString name;
    QString version;
};


#endif //JETBRAINSRUNNER_SETTINGSDIRECTORY_H
