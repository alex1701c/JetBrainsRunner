#ifndef JETBRAINSRUNNER_JETBRAINSRUNNER_CONFIG_MAPPING_ITEM_H
#define JETBRAINSRUNNER_JETBRAINSRUNNER_CONFIG_MAPPING_ITEM_H

#include <QtWidgets/QFileDialog>
#include <QBitmap>
#include <JetbrainsApplication.h>
#include "ui_jetbrainsrunner_config_mapping_item.h"

class JetbrainsRunnerConfigMappingItem : public QWidget, public Ui::JetbrainsRunnerConfigItemUi {
Q_OBJECT

public:
    explicit JetbrainsRunnerConfigMappingItem(QWidget *parent, const QString& desktopFilePath = "", const QString& configFilePath = "")
            : QWidget(parent) {
        setupUi(this);

        if (!desktopFilePath.isEmpty() && !configFilePath.isEmpty()) {
            this->configDesktoFilePushButton->setText(desktopFilePath);
            this->configXMLFilePushButton->setText(configFilePath);
        }

        connect(this->deletePushButton, SIGNAL(clicked(bool)), parent, SLOT(changed()));
        connect(this->deletePushButton, SIGNAL(clicked(bool)), parent, SLOT(deleteMappingItem()));
        connect(this->configDesktoFilePushButton, SIGNAL(clicked(bool)), parent, SLOT(changed()));
        connect(this->configDesktoFilePushButton, SIGNAL(clicked(bool)), this, SLOT(openDesktopFileChooser()));
        connect(this->configXMLFilePushButton, SIGNAL(clicked(bool)), parent, SLOT(changed()));
        connect(this->configXMLFilePushButton, SIGNAL(clicked(bool)), this, SLOT(openConfigFileChooser()));

        validateInfoWidget();
    }

public Q_SLOTS:

    void openDesktopFileChooser() {
        QString desktopFile = QFileDialog::getOpenFileName(this, tr("Select Desktop File"), "",
                                                           tr("Desktop File (*.desktop)"));
        if (!desktopFile.isEmpty()) {
            this->configDesktoFilePushButton->setText(desktopFile);
        }
        validateInfoWidget();
    }

    void openConfigFileChooser() {
        QString configLocation =
                QFileDialog::getOpenFileName(this, tr("Select Config File"), "",
                                             tr("XML File (*.xml)"));
        if (!configLocation.isEmpty()) {
            this->configXMLFilePushButton->setText(configLocation);
        }
        validateInfoWidget();
    }

    void validateInfoWidget() {
        const QString desktopFile = this->configDesktoFilePushButton->text().remove("&");
        const QString configFile = this->configXMLFilePushButton->text().remove("&");
        if (QFile::exists(desktopFile)) {
            this->configTitleWidget->setHidden(false);
            JetbrainsApplication newApp(desktopFile);
            this->appNameLabel->setText(newApp.name
                                                .remove(" Release").remove(" Edition")
                                                .remove(" + JBR11").remove(" RC").remove(" EAP"));
            QPixmap pixmap(newApp.iconPath);
            this->applicationIconLabel->setPixmap(pixmap);
            this->applicationIconLabel->setMask(pixmap.mask());
        } else {
            this->configTitleWidget->setHidden(true);
        }
        if (QFile::exists(configFile)) {
            QFile file(configFile);
            if (file.open(QFile::ReadOnly)) {
                this->configReadWidget->setHidden(false);
                JetbrainsApplication newApp(desktopFile);
                newApp.parseXMLFile(file.readAll());

                QString recentApplicationsText;
                for (const auto &recentProject: newApp.recentlyUsed) {
                    recentApplicationsText.append(recentProject + "\n");
                }
                this->recentProjectsLabel->setText(recentApplicationsText);
            }
        } else {
            this->configReadWidget->setHidden(true);
        }
    }
};

#endif //JETBRAINSRUNNER_JETBRAINSRUNNER_CONFIG_MAPPING_ITEM_H
