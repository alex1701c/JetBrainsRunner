#include "jetbrainsrunner_config.h"
#include "jetbrainsrunner_config_mapping_item.h"
#include <KSharedConfig>
#include <KPluginFactory>
#include <QJsonDocument>
#include <QJsonArray>
#include <QtWidgets/QLabel>

#include "jetbrains-api/SettingsDirectory.h"
#include "jetbrains-api/ConfigKeys.h"

K_PLUGIN_CLASS(JetbrainsRunnerConfig)

JetbrainsRunnerConfigForm::JetbrainsRunnerConfigForm(QWidget *parent) : QWidget(parent) {
    setupUi(this);
}

JetbrainsRunnerConfig::JetbrainsRunnerConfig(QObject *parent, const QVariantList &)
    : KCModule(qobject_cast<QWidget*>(parent)) {
    m_ui = new JetbrainsRunnerConfigForm(widget());
    m_ui->setMinimumWidth(600);
    auto *layout = new QGridLayout(widget());
    layout->addWidget(m_ui, 0, 0);

    config = KSharedConfig::openConfig(QStringLiteral("krunnerrc"))->group("Runners").group("krunner_jetbrainsrunner");
    customMappingGroup = config.group(QStringLiteral("CustomMapping"));

    const auto changedSlotPointer = &JetbrainsRunnerConfig::markAsChanged;
    connect(m_ui->appNameSearch, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->appNameSearch, &QCheckBox::clicked, this, &JetbrainsRunnerConfig::validateOptions);
    connect(m_ui->projectNameSearch, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->projectNameSearch, &QCheckBox::clicked, this, &JetbrainsRunnerConfig::validateOptions);
    connect(m_ui->updatesCheckBox, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->filterSearchResultsComboBox, &QComboBox::currentTextChanged, this, changedSlotPointer);
    connect(m_ui->newManualMappingPushButton, &QPushButton::clicked, this, &JetbrainsRunnerConfig::addNewMappingItem);
    connect(m_ui->newManualMappingPushButton, &QPushButton::clicked, this, changedSlotPointer);
    connect(m_ui->formatStringLineEdit, &QLineEdit::textChanged, this, changedSlotPointer);
    connect(m_ui->formatStringLineEdit, &QLineEdit::textChanged, this, &JetbrainsRunnerConfig::validateFormattingString);
    connect(m_ui->defaultFormattingPushButton, &QPushButton::clicked, this, &JetbrainsRunnerConfig::setDefaultFormatting);
    connect(m_ui->defaultFormattingPushButton, &QPushButton::clicked, this, changedSlotPointer);
    connect(m_ui->showCategoryCheckBox, &QCheckBox::clicked, this, changedSlotPointer);
    connect(m_ui->logFilePushButton, &QPushButton::clicked, this, &JetbrainsRunnerConfig::exportDebugFile);
    validateOptions();
}


void JetbrainsRunnerConfig::load() {
    m_ui->filterSearchResultsComboBox->insertItem(0, QStringLiteral("Path Contains"), (int) SearchResultChoice::PathContains);
    m_ui->filterSearchResultsComboBox->insertItem(0, QStringLiteral("Project Contains"), (int) SearchResultChoice::ProjectNameContains);
    m_ui->filterSearchResultsComboBox->insertItem(0, QStringLiteral("Project Startswith (default)"), (int) SearchResultChoice::ProjectNameStartsWith);
    m_ui->formatStringValidationLabel->setHidden(true);
    m_ui->formatStringValidationLabel->setStyleSheet(QStringLiteral("QLabel { color : red }"));
    m_ui->defaultFormattingPushButton->setHidden(true);
    m_ui->appNameSearch->setChecked(config.readEntry(Config::launchByAppName, true));
    m_ui->projectNameSearch->setChecked(config.readEntry(Config::launchByProjectName, true));
    m_ui->updatesCheckBox->setChecked(config.readEntry(Config::notifyUpdates, true));
    m_ui->showCategoryCheckBox->setChecked(config.readEntry(Config::displayInCategories, false));
    m_ui->formatStringLineEdit->setText(config.readEntry(Config::formatString, Config::formatStringDefault));
    int enumValue = config.readEntry(Config::filterSearchResults, (int) SearchResultChoice::ProjectNameStartsWith);
    const int idx = m_ui->filterSearchResultsComboBox->findData(enumValue);
    m_ui->filterSearchResultsComboBox->setCurrentIndex(idx != -1 ? idx : 0);
    if (m_ui->updatesCheckBox->isChecked()) {
        makeVersionRequest();
    }

    const auto entries = customMappingGroup.entryMap().toStdMap();
    for (const auto &entry: entries) {
        const auto item = new JetbrainsRunnerConfigMappingItem(widget(), entry.first, entry.second);
        const auto changedSlotPointer = &JetbrainsRunnerConfig::markAsChanged;
        connect(item, &JetbrainsRunnerConfigMappingItem::changed, this, changedSlotPointer);
        connect(item, &JetbrainsRunnerConfigMappingItem::deleteMappingItem, this, &JetbrainsRunnerConfig::deleteMappingItem);
        m_ui->manualMappingVBox->addWidget(item);
    }
}

void JetbrainsRunnerConfig::save() {
    config.writeEntry(Config::launchByAppName, m_ui->appNameSearch->isChecked());
    config.writeEntry(Config::launchByProjectName, m_ui->projectNameSearch->isChecked());
    config.writeEntry(Config::notifyUpdates, m_ui->updatesCheckBox->isChecked());
    config.writeEntry(Config::formatString, m_ui->formatStringLineEdit->text());
    config.writeEntry(Config::displayInCategories, m_ui->showCategoryCheckBox->isChecked());
    config.writeEntry(Config::filterSearchResults, m_ui->filterSearchResultsComboBox->currentData());

    const int itemCount = m_ui->manualMappingVBox->count();
    // Reset config by deleting group
    customMappingGroup.deleteGroup();
    // Write items to config
    for (int i = 0; i < itemCount; ++i) {
        auto *item = reinterpret_cast<JetbrainsRunnerConfigMappingItem *>(m_ui->manualMappingVBox->itemAt(i)->widget());
        const QString desktopFilePath = item->configDesktoFilePushButton->text().remove('&');
        const QString configFilePath = item->configXMLFilePushButton->text().remove('&');
        if (QFileInfo::exists(desktopFilePath) && QFileInfo::exists(configFilePath)) {
            customMappingGroup.writeEntry(desktopFilePath, configFilePath);
        }
    }

    config.config()->sync();

    markAsChanged();
}

void JetbrainsRunnerConfig::defaults() {
    m_ui->appNameSearch->setChecked(true);
    m_ui->projectNameSearch->setChecked(true);
    m_ui->formatStringLineEdit->setText(Config::formatStringDefault);
    m_ui->showCategoryCheckBox->setChecked(false);
    m_ui->filterSearchResultsComboBox->setCurrentIndex(0);
    markAsChanged();
}

void JetbrainsRunnerConfig::validateOptions() {
    m_ui->appNameSearch->setDisabled(m_ui->appNameSearch->isChecked() && !m_ui->projectNameSearch->isChecked());
    m_ui->projectNameSearch->setDisabled(m_ui->projectNameSearch->isChecked() && !m_ui->appNameSearch->isChecked());
}

void JetbrainsRunnerConfig::makeVersionRequest() {
    auto manager = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl(QStringLiteral("https://api.github.com/repos/alex1701c/JetBrainsRunner/releases")));
    manager->get(request);
    connect(manager, &QNetworkAccessManager::finished, this, &JetbrainsRunnerConfig::displayUpdateNotification);
}

void JetbrainsRunnerConfig::exportDebugFile() {
    const QString filename = QFileDialog::getSaveFileName(
            widget(), QStringLiteral("Save file"), QString(), QStringLiteral(".txt"));
    if (filename.isEmpty()) {
        return;
    }
    auto debugString = new QString();
    const auto mappingMap = config.group("CustomMapping").entryMap();
    QList<JetbrainsApplication *> appList;
    QList<JetbrainsApplication *> automaticAppList;
    auto desktopPaths = JetbrainsApplication::getInstalledApplicationPaths(config.group("CustomMapping"), debugString);

    // Split manually configured and automatically found apps
    if (!mappingMap.isEmpty()) {
        debugString->append("========== Custom Configured Applications ==========\n");
    }
    for (const auto &p:desktopPaths.toStdMap()) {
        // Desktop file is manually specified
        if (mappingMap.contains(p.second)) {
            auto customMappedApp = new JetbrainsApplication(p.second);
            QFile xmlConfigFile(mappingMap.value(p.second));
            if (xmlConfigFile.open(QFile::ReadOnly)) {
                customMappedApp->parseXMLFile(xmlConfigFile.readAll(), debugString);
                // Add path for filewatcher
                customMappedApp->addPath(mappingMap.value(p.second));
                if (!customMappedApp->recentlyUsed.isEmpty()) {
                    appList.append(customMappedApp);
                }
            }
            xmlConfigFile.close();
        } else {
            automaticAppList.append(new JetbrainsApplication(p.second));
        }
    }

    SettingsDirectory::findCorrespondingDirectories(SettingsDirectory::getSettingsDirectories(debugString),
            automaticAppList, debugString);
    JetbrainsApplication::parseXMLFiles(automaticAppList, debugString);
    automaticAppList = JetbrainsApplication::filterApps(automaticAppList, debugString);

    auto *f = new QFile(filename);
    f->open(QIODevice::WriteOnly | QIODevice::Text);
    f->write(debugString->toLocal8Bit());
    f->flush();
    f->close();
}

void JetbrainsRunnerConfig::displayUpdateNotification(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QString displayText;
        const auto jsonObject = QJsonDocument::fromJson(reply->readAll());
        if (jsonObject.isArray()) {
            const auto jsonArray = jsonObject.array();
            for (const auto &githubReleaseObj:jsonArray) {
                if (githubReleaseObj.isObject()) {
                    const auto githubRelease = githubReleaseObj.toObject();
                    const QString tagName= githubRelease.value(QLatin1String("tag_name")).toString();
                    if (tagName > QLatin1String(CMAKE_PROJECT_VERSION)) {
                        const QString name = githubRelease.value(QLatin1String("name")).toString();
                        displayText.append(tagName + ": " + name + "\n");
                    }
                }
            }
        }
        if (!displayText.isEmpty()) {
            displayText.prepend(QStringLiteral("Current Version: %1\n").arg(QLatin1String(CMAKE_PROJECT_VERSION)));
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
    auto *item = new JetbrainsRunnerConfigMappingItem(widget());
    m_ui->manualMappingVBox->addWidget(item);
    connect(item, &JetbrainsRunnerConfigMappingItem::deleteMappingItem, this, &JetbrainsRunnerConfig::deleteMappingItem);
}

void JetbrainsRunnerConfig::deleteMappingItem() {
    this->sender()->deleteLater();
}

void JetbrainsRunnerConfig::validateFormattingString() {
    const QString text = m_ui->formatStringLineEdit->text();
    if (text.isEmpty()) {
        m_ui->formatStringValidationLabel->setText(QStringLiteral("The formatting string can not be empty"));
        m_ui->formatStringValidationLabel->setHidden(false);
        m_ui->defaultFormattingPushButton->setHidden(false);
    } else if (!text.contains(QLatin1String(FormatString::DIR)) && !text.contains(QLatin1String(FormatString::PROJECT))) {
        m_ui->formatStringValidationLabel->setText(QStringLiteral("The formatting string must contain %1 or %2")
        .arg(QLatin1String(FormatString::DIR), QLatin1String(FormatString::PROJECT)));
        m_ui->formatStringValidationLabel->setHidden(false);
        m_ui->defaultFormattingPushButton->setHidden(true);
    } else {
        m_ui->formatStringValidationLabel->setHidden(true);
        m_ui->defaultFormattingPushButton->setHidden(true);
    }
}

void JetbrainsRunnerConfig::setDefaultFormatting() {
    m_ui->formatStringLineEdit->setText(Config::formatStringDefault);
}


#include "jetbrainsrunner_config.moc"
#include "moc_jetbrainsrunner_config.cpp"
#include "moc_jetbrainsrunner_config_mapping_item.cpp"
