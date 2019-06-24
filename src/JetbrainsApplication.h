//
// Created by alex on 24.06.19.
//

#ifndef JETBRAINSAPPLICATION_H
#define JETBRAINSAPPLICATION_H


#include <QtCore/QStringList>
#include <ostream>

class JetbrainsApplication {
public:

    JetbrainsApplication(const QString &desktopFilePath);

    QStringList recentlyUsed;
    QString desktopFilePath;
    QString executablePath;
    QString iconPath;
    QString name = "";

};


#endif //JETBRAINSAPPLICATION_H
