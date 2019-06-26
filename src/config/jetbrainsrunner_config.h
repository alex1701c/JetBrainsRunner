/******************************************************************************
 *   Copyright 2019 by Alex alexkp12355@gmail.com                             *
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
