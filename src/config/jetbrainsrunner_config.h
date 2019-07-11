#ifndef JetbrainsRunnerCONFIG_H
#define JetbrainsRunnerCONFIG_H

#include "ui_jetbrainsrunner_config.h"
#include <KCModule>
#include <KConfigCore/KConfigGroup>

class JetbrainsRunnerConfigForm : public QWidget, public Ui::JetbrainsRunnerConfigUi {
Q_OBJECT

public:
    explicit JetbrainsRunnerConfigForm(QWidget *parent);
};

class JetbrainsRunnerConfig : public KCModule {
Q_OBJECT

public:
    explicit JetbrainsRunnerConfig(QWidget *parent = nullptr, const QVariantList &args = QVariantList());

    KConfigGroup config;

public Q_SLOTS:

    void save() override;

    void defaults() override;

private:
    JetbrainsRunnerConfigForm *m_ui;

};

#endif
