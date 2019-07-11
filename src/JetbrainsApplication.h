#ifndef JETBRAINSAPPLICATION_H
#define JETBRAINSAPPLICATION_H


#include <QtCore/QStringList>
#include <ostream>

class JetbrainsApplication {
public:

    explicit JetbrainsApplication(const QString &desktopFilePath);

    static QList<JetbrainsApplication> getInstalledList();

    static void parseXMLFiles(QList<JetbrainsApplication> &apps);

    void parseXMLFile();

    static QList<JetbrainsApplication> filterApps(QList<JetbrainsApplication> &apps);

    QList<QString> recentlyUsed;
    QString desktopFilePath;
    QString executablePath;
    QString iconPath;
    QString name = "";
    QString configFolder = "";

};


#endif //JETBRAINSAPPLICATION_H
