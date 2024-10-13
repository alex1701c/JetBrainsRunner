#ifndef JetbrainsRunnerCONFIG_H
#define JetbrainsRunnerCONFIG_H

#include "ui_jetbrainsrunner_config.h"
#include <KCModule>
#include <KConfigGroup>
#include <QtNetwork/QNetworkReply>
#include <kcmutils_version.h>

class JetbrainsRunnerConfigForm : public QWidget, public Ui::JetbrainsRunnerConfigUi
{
    Q_OBJECT

public:
    explicit JetbrainsRunnerConfigForm(QWidget *parent);
};

class JetbrainsRunnerConfig : public KCModule
{
    Q_OBJECT

public:
    explicit JetbrainsRunnerConfig(QObject *parent, const QVariantList &);

    KConfigGroup config;
    KConfigGroup customMappingGroup;

public Q_SLOTS:
    void exportDebugFile();

    void makeVersionRequest();
    void displayUpdateNotification(QNetworkReply *reply);

    void addNewMappingItem();
    void deleteMappingItem();

    void validateOptions();
    void validateFormattingString();
    void setDefaultFormatting();

    void load() override;
    void save() override;
    void defaults() override;
#if KCMUTILS_VERSION < QT_VERSION_CHECK(5, 106, 0)
    inline QWidget *widget()
    {
        return this;
    }
#endif

private:
    JetbrainsRunnerConfigForm *m_ui;
};

#endif
