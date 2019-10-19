#ifndef JETBRAINSRUNNER_JETBRAINSRUNNER_CONFIG_MAPPING_ITEM_H
#define JETBRAINSRUNNER_JETBRAINSRUNNER_CONFIG_MAPPING_ITEM_H

#include <QtWidgets/QFileDialog>
#include <QBitmap>
#include <JetbrainsApplication.h>
#include "ui_jetbrainsrunner_config_mapping_item.h"

class JetbrainsRunnerConfigMappingItem : public QWidget, public Ui::JetbrainsRunnerConfigItemUi {
Q_OBJECT

public:
    explicit JetbrainsRunnerConfigMappingItem(QWidget *parent) : QWidget(parent) {
        setupUi(this);
        this->parentComponent = parent;

        connect(this->deletePushButton, SIGNAL(clicked(bool)), parent, SLOT(changed()));
        connect(this->deletePushButton, SIGNAL(clicked(bool)), parent, SLOT(deleteMappingItem()));
        connect(this->configDesktoFilePushButton, SIGNAL(clicked(bool)), parent, SLOT(changed()));
        connect(this->configDesktoFilePushButton, SIGNAL(clicked(bool)), this, SLOT(openDesktopFileChooser()));
        connect(this->configFolderPushButton, SIGNAL(clicked(bool)), parent, SLOT(changed()));
        connect(this->configFolderPushButton, SIGNAL(clicked(bool)), this, SLOT(openConfigFileChooser()));

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
            this->configFolderPushButton->setText(configLocation);
        }
        validateInfoWidget();
    }

    void validateInfoWidget() {
        const QString desktopFile = this->configDesktoFilePushButton->text().remove("&");
        const QString configFile = this->configFolderPushButton->text().remove("&");
        if (QFile::exists(desktopFile) && QFile::exists(configFile)) {
            this->configReadWidget->setHidden(false);
            JetbrainsApplication newApp(desktopFile);
            QFile file(configFile);
            if (file.open(QFile::ReadOnly)) {
                newApp.parseXMLFile(file.readAll());
                this->appNameLabel->setText(newApp.name
                                                    .remove(" Release").remove(" Edition")
                                                    .remove(" + JBR11").remove(" RC").remove(" EAP"));
                QPixmap pixmap(newApp.iconPath);
                this->applicationIconLabel->setPixmap(pixmap);
                this->applicationIconLabel->setMask(pixmap.mask());

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

private:
    QWidget *parentComponent;
};

#endif //JETBRAINSRUNNER_JETBRAINSRUNNER_CONFIG_MAPPING_ITEM_H
