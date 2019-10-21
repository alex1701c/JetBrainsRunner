#ifndef JETBRAINSAPPLICATION_H
#define JETBRAINSAPPLICATION_H

//#define LOG_INSTALLED

#include <QtCore>
#include <KConfigCore/KConfigGroup>

class JetbrainsApplication : public QFileSystemWatcher {
Q_OBJECT
public:

    explicit JetbrainsApplication(const QString &desktopFilePath);

    static QMap<QString, QString> getInstalledApplicationPaths(const KConfigGroup &customMappingConfig);

    static void parseXMLFiles(QList<JetbrainsApplication *> &apps);

    void parseXMLFile(QString fileContent = "");

    static QList<JetbrainsApplication *> filterApps(QList<JetbrainsApplication *> &apps);

    static QStringList getAdditionalDesktopFileLocations();

    static QString filterApplicationName(const QString &name);


    QList<QString> recentlyUsed;
    QString desktopFilePath;
    QString executablePath;
    QString iconPath;
    QString name = "";
    QString configFolder = "";
    bool secondName = false;
    QString nameArray[2] = {"", ""};

public Q_SLOTS:

    void configChanged(const QString &file) {
        QFile f(file);
        if (f.exists() && f.open(QIODevice::ReadOnly)) {
            this->recentlyUsed.clear();
            QString content = f.readAll();
            f.close();
            this->parseXMLFile(content);
        }
#ifdef LOG_INSTALLED
            qInfo() << "File Changed Event: " << file;
#endif
        qInfo() << "Changed config: " << file;
    };

};


#endif //JETBRAINSAPPLICATION_H
