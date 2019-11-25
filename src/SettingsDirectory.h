#ifndef JETBRAINSRUNNER_SETTINGSDIRECTORY_H
#define JETBRAINSRUNNER_SETTINGSDIRECTORY_H


#include "JetbrainsApplication.h"

class SettingsDirectory {
public:

    static QMap<QString, QString> getAliases();

    SettingsDirectory(QString directory, QString name);

    static QList<SettingsDirectory> getSettingsDirectories(QString *debugMessage = nullptr);

    static void findCorrespondingDirectory(const QList<SettingsDirectory> &dirs, JetbrainsApplication *app);

    static void findCorrespondingDirectories(const QList<SettingsDirectory> &dirs, QList<JetbrainsApplication *> &apps);


    QString directory;
    QString name;
};


#endif //JETBRAINSRUNNER_SETTINGSDIRECTORY_H
