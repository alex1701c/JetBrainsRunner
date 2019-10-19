#ifndef JetbrainsRunnerCONFIG_H
#define JetbrainsRunnerCONFIG_H

#include "ui_jetbrainsrunner_config.h"
#include <KCModule>
#include <KConfigCore/KConfigGroup>
#include <QtNetwork/QNetworkReply>

class JetbrainsRunnerConfigForm : public QWidget, public Ui::JetbrainsRunnerConfigUi {
Q_OBJECT

public:
    explicit JetbrainsRunnerConfigForm(QWidget *parent);
};

class JetbrainsRunnerConfig : public KCModule {
Q_OBJECT

public:
    explicit JetbrainsRunnerConfig(QWidget *parent = nullptr, const QVariantList &args = QVariantList());

    void makeVersionRequest();

    KConfigGroup config;

public Q_SLOTS:

    void displayUpdateNotification(QNetworkReply *reply);

    void addNewMappingItem();

    void deleteMappingItem();

    void validateOptions();

    void save() override;

    void defaults() override;

private:
    JetbrainsRunnerConfigForm *m_ui;

};

#endif
