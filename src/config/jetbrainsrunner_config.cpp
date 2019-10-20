#include "jetbrainsrunner_config.h"
#include "jetbrainsrunner_config_mapping_item.h"
#include <KSharedConfig>
#include <KPluginFactory>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QtWidgets/QLabel>

K_PLUGIN_FACTORY(JetbrainsRunnerConfigFactory, registerPlugin<JetbrainsRunnerConfig>("kcm_krunner_jetbrainsrunner");)

JetbrainsRunnerConfigForm::JetbrainsRunnerConfigForm(QWidget *parent) : QWidget(parent) {
    setupUi(this);
}

JetbrainsRunnerConfig::JetbrainsRunnerConfig(QWidget *parent, const QVariantList &args) : KCModule(parent, args) {
    m_ui = new JetbrainsRunnerConfigForm(this);
    m_ui->setMinimumWidth(400);
    auto *layout = new QGridLayout(this);
    layout->addWidget(m_ui, 0, 0);

    config = KSharedConfig::openConfig("krunnerrc")->group("Runners").group("JetBrainsRunner");
    customMappingGroup = config.group("CustomMapping");

    connect(m_ui->appNameSearch, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->appNameSearch, SIGNAL(clicked(bool)), this, SLOT(validateOptions()));
    connect(m_ui->projectNameSearch, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->projectNameSearch, SIGNAL(clicked(bool)), this, SLOT(validateOptions()));
    connect(m_ui->updatesCheckBox, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->newManualMappingPushButton, SIGNAL(clicked(bool)), this, SLOT(addNewMappingItem()));
    connect(m_ui->newManualMappingPushButton, SIGNAL(clicked(bool)), this, SLOT(changed()));
    validateOptions();
}


void JetbrainsRunnerConfig::load() {
    m_ui->appNameSearch->setChecked(config.readEntry("LaunchByAppName", "true") == "true");
    m_ui->projectNameSearch->setChecked(config.readEntry("LaunchByProjectName", "true") == "true");
    m_ui->updatesCheckBox->setChecked(config.readEntry("NotifyUpdates", "true") == "true");
    if (m_ui->updatesCheckBox->isChecked()) {
        makeVersionRequest();
    }

    for (const auto &entry: customMappingGroup.entryMap().toStdMap()) {
        m_ui->manualMappingVBox->addWidget(new JetbrainsRunnerConfigMappingItem(this, entry.first, entry.second));
    }
}

void JetbrainsRunnerConfig::save() {
    config.writeEntry("LaunchByAppName", m_ui->appNameSearch->isChecked());
    config.writeEntry("LaunchByProjectName", m_ui->projectNameSearch->isChecked());
    config.writeEntry("NotifyUpdates", m_ui->updatesCheckBox->isChecked());


    const int itemCount = m_ui->manualMappingVBox->count();
    // Reset config by deleting group
    customMappingGroup.deleteGroup();
    // Write items to config
    for (int i = 0; i < itemCount; ++i) {
        auto *item = reinterpret_cast<JetbrainsRunnerConfigMappingItem *>(m_ui->manualMappingVBox->itemAt(i)->widget());
        const QString desktopFilePath = item->configDesktoFilePushButton->text().remove("&");
        const QString configFilePath = item->configXMLFilePushButton->text().remove("&");
        if (QFile::exists(desktopFilePath) && QFile::exists(configFilePath)) {
            customMappingGroup.writeEntry(desktopFilePath, configFilePath);
        }
    }

    emit changed();
}

void JetbrainsRunnerConfig::defaults() {
    m_ui->appNameSearch->setChecked(true);
    m_ui->projectNameSearch->setChecked(true);
    emit changed(true);
}

void JetbrainsRunnerConfig::validateOptions() {
    m_ui->appNameSearch->setDisabled(m_ui->appNameSearch->isChecked() && !m_ui->projectNameSearch->isChecked());
    m_ui->projectNameSearch->setDisabled(m_ui->projectNameSearch->isChecked() && !m_ui->appNameSearch->isChecked());
}

void JetbrainsRunnerConfig::makeVersionRequest() {
    auto manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl("https://api.github.com/repos/alex1701c/JetBrainsRunner/releases"));
    manager->get(request);
    connect(manager, SIGNAL(finished(QNetworkReply * )), this, SLOT(displayUpdateNotification(QNetworkReply * )));
}

void JetbrainsRunnerConfig::displayUpdateNotification(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QString displayText;
        auto jsonObject = QJsonDocument::fromJson(reply->readAll());
        if (jsonObject.isArray()) {
            for (const auto &githubReleaseObj:jsonObject.array()) {
                if (githubReleaseObj.isObject()) {
                    auto githubRelease = githubReleaseObj.toObject();
                    if (githubRelease.value("tag_name").toString() > "1.2.1") {
                        displayText.append(githubRelease.value("tag_name").toString() + ": " +
                                           githubRelease.value("name").toString() + "\n");
                    }
                }
            }
        }
        if (!displayText.isEmpty()) {
            displayText.prepend("Current Version: 1.2.1\n");
            displayText.prepend("<pre>Updates Available !\n");
            displayText.append("Please go to <a href=\"https://github.com/alex1701c/JetBrainsRunner\">"\
                               "https://github.com/alex1701c/JetBrainsRunner</a>\nand get the latest version</pre>");
            auto displayLabel = new QLabel(displayText, m_ui->parentLayout);
            displayLabel->setTextFormat(Qt::RichText);
            displayLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
            displayLabel->setOpenExternalLinks(true);
            m_ui->verticalLayout_2->insertWidget(0, displayLabel);
        }
    }
}

void JetbrainsRunnerConfig::addNewMappingItem() {
    m_ui->manualMappingVBox->addWidget(new JetbrainsRunnerConfigMappingItem(this));
}


void JetbrainsRunnerConfig::deleteMappingItem() {
    this->sender()->parent()->deleteLater();
}


#include "jetbrainsrunner_config.moc"
