/******************************************************************************
 *   Copyright %{CURRENT_YEAR} by %{AUTHOR} <%{EMAIL}>                        *
 *                                                                            *
 *  This library is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU Lesser General Public License as published  *
 *  by the Free Software Foundation; either version 2 of the License or (at   *
 *  your option) any later version.                                           *
 *                                                                            *
 *  This library is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 *  Library General Public License for more details.                          *
 *                                                                            *
 *  You should have received a copy of the GNU Lesser General Public License  *
 *  along with this library; see the file COPYING.LIB.                        *
 *  If not, see <http://www.gnu.org/licenses/>.                               *
 *****************************************************************************/

#include "jetbrainsrunner_config.h"
#include <KSharedConfig>
#include <KPluginFactory>
#include <krunner/abstractrunner.h>
#include <QtCore/QProcess>
#include <QtWidgets/QGridLayout>

K_PLUGIN_FACTORY(JetbrainsRunnerConfigFactory, registerPlugin<JetbrainsRunnerConfig>("kcm_krunner_jetbrainsrunner");)

JetbrainsRunnerConfigForm::JetbrainsRunnerConfigForm(QWidget *parent) : QWidget(parent) {
    setupUi(this);
}

JetbrainsRunnerConfig::JetbrainsRunnerConfig(QWidget *parent, const QVariantList &args) : KCModule(parent, args) {
    m_ui = new JetbrainsRunnerConfigForm(this);
    auto *layout = new QGridLayout(this);
    layout->addWidget(m_ui, 0, 0);

    config = KSharedConfig::openConfig("krunnerrc")->group("Runners").group("JetBrainsRunner");

    m_ui->appNameSearch->setChecked(config.readEntry("LaunchByAppName", "true") == "true");
    m_ui->projectNameSearch->setChecked(config.readEntry("LaunchByProjectName", "true") == "true");

    connect(m_ui->appNameSearch, SIGNAL(clicked(bool)), this, SLOT(changed()));
    connect(m_ui->projectNameSearch, SIGNAL(clicked(bool)), this, SLOT(changed()));


    load();

}


void JetbrainsRunnerConfig::save() {

    KCModule::save();

    config.writeEntry("LaunchByAppName", m_ui->appNameSearch->isChecked() ? "true" : "false");
    config.writeEntry("LaunchByProjectName", m_ui->projectNameSearch->isChecked() ? "true" : "false");

    emit changed();
}

void JetbrainsRunnerConfig::defaults() {
    m_ui->appNameSearch->setChecked(true);
    m_ui->projectNameSearch->setChecked(true);
    emit changed(true);
}


#include "jetbrainsrunner_config.moc"
