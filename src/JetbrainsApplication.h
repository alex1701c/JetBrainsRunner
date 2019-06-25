//
// Created by alex on 24.06.19.
//

#ifndef JETBRAINSAPPLICATION_H
#define JETBRAINSAPPLICATION_H


#include <QtCore/QStringList>
#include <ostream>

class JetbrainsApplication {
public:

    explicit JetbrainsApplication(const QString &desktopFilePath);

    static void parseXMLFiles(QList<JetbrainsApplication> &apps);

    void parseXMLFile();

    QList<QString> recentlyUsed;
    QString desktopFilePath;
    QString executablePath;
    QString iconPath;
    QString name = "";
    QString configFolder = "";

};




#endif //JETBRAINSAPPLICATION_H
