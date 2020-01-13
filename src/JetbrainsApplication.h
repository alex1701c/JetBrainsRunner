#ifndef JETBRAINSAPPLICATION_H
#define JETBRAINSAPPLICATION_H

#include <QtCore>
#include <KConfigCore/KConfigGroup>

class JetbrainsApplication : public QFileSystemWatcher {
Q_OBJECT
public:

    explicit JetbrainsApplication(const QString &desktopFilePath);

    static QMap<QString, QString>
    getInstalledApplicationPaths(const KConfigGroup &customMappingConfig, QString *debugMessage = nullptr);

    static void parseXMLFiles(QList<JetbrainsApplication *> &apps, QString *debugMessage = nullptr);

    void parseXMLFile(QString fileContent = "", QString *debugMessage = nullptr);

    static QList<JetbrainsApplication *> filterApps(QList<JetbrainsApplication *> &apps, QString *debugMessage = nullptr);

    static QStringList getAdditionalDesktopFileLocations();

    static QString filterApplicationName(const QString &name);

    QString formatOptionText(const QString &formatText, const QString &dir);


    QList<QString> recentlyUsed;
    QString desktopFilePath;
    QString executablePath;
    QString iconPath;
    QString name = "";
    QString configFolder = "";
    QString shortName = "";
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
    };
};


#endif //JETBRAINSAPPLICATION_H
