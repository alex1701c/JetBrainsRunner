#ifndef JETBRAINSAPPLICATION_H
#define JETBRAINSAPPLICATION_H

//#define LOG_INSTALLED

#include <QtCore>

class JetbrainsApplication : public QFileSystemWatcher {
Q_OBJECT
public:

    explicit JetbrainsApplication(const QString &desktopFilePath);

    static QList<JetbrainsApplication *> getInstalledList();

    static void parseXMLFiles(QList<JetbrainsApplication *> &apps);

    void parseXMLFile(QString fileContent = "");

    static QList<JetbrainsApplication *> filterApps(QList<JetbrainsApplication *> &apps);

    QList<QString> recentlyUsed;
    QString desktopFilePath;
    QString executablePath;
    QString iconPath;
    QString name = "";
    QString configFolder = "";

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
