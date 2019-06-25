/*
   Copyright %{CURRENT_YEAR} by %{AUTHOR} <%{EMAIL}>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "jetbrainsrunner.h"

// KF
#include <KLocalizedString>
#include <QtGui/QtGui>
#include <iostream>
#include <QtXml/QDomDocument>

//TODO Filter unmatched
//TODO Remove dublicate folder names

JetbrainsRunner::JetbrainsRunner(QObject *parent, const QVariantList &args)
        : Plasma::AbstractRunner(parent, args) {
    setObjectName(QStringLiteral("JetbrainsRunner"));
}

#include "JetbrainsApplication.h"
#include "SettingsDirectory.h"

JetbrainsRunner::~JetbrainsRunner() {
}

void JetbrainsRunner::init() {

    installed = JetbrainsApplication::getInstalledList();
    QList<SettingsDirectory> dirs = SettingsDirectory::getSettingsDirectories();
    SettingsDirectory::findCorrespondingDirectories(dirs, installed);
    JetbrainsApplication::parseXMLFiles(installed);
    installed = JetbrainsApplication::filterApps(installed);

    for (const auto &i:installed) {
        qInfo() << "\n<------------ " << i.name << " ------------------->";
        for (const auto &d:i.recentlyUsed) {
            qInfo() << d;
        }
    }
}

/*
 * How to run matches:
 *  1: Name of application matches
 *  2: Name of project matches
 */
void JetbrainsRunner::match(Plasma::RunnerContext &context) {
    const QString term = context.query().toLower();
    QList<Plasma::QueryMatch> matches;

    for (auto const &app:installed) {
        if (QString(app.name).replace(" ", "").toLower().startsWith(term)) {
            matches.append(addAppNameMatches(app, term));
        }
    }

    context.addMatches(matches);
}

void JetbrainsRunner::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) {
    Q_UNUSED(context)
    Q_UNUSED(match)

    QProcess::startDetached(match.data().toString());
}

QList<Plasma::QueryMatch> JetbrainsRunner::addAppNameMatches(const JetbrainsApplication &app, const QString &term) {
    QList<Plasma::QueryMatch> matches;
    for (const auto &dir:app.recentlyUsed) {
        Plasma::QueryMatch match(this);
        match.setText(app.name + " Launch " + dir.split('/').last());
        match.setIconName(app.iconPath);
        match.setData(QString(app.executablePath).replace("%f", dir));
        match.setRelevance((float) term.length() / (float) app.name.length());
        matches.append(match);
    }
    return matches;
}

K_EXPORT_PLASMA_RUNNER(jetbrainsrunner, JetbrainsRunner)

// needed for the QObject subclass declared as part of K_EXPORT_PLASMA_RUNNER
#include "jetbrainsrunner.moc"
