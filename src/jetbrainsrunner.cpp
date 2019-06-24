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

JetbrainsRunner::JetbrainsRunner(QObject *parent, const QVariantList &args)
        : Plasma::AbstractRunner(parent, args) {
    setObjectName(QStringLiteral("JetbrainsRunner"));
}

JetbrainsRunner::~JetbrainsRunner() {
}

void JetbrainsRunner::init() {
    // Apps can be installed using snap, their toolbox app and by downloading from the website
    QStringList installed;
    QProcess toolBoxProcess;
    toolBoxProcess.start("sh", QStringList() << "-c"
                                             << "ls ~/.local/share/applications | grep jetbrains");
    toolBoxProcess.waitForFinished();
    for (const auto &item :toolBoxProcess.readAllStandardOutput().split('\n')) {
        if (!item.isEmpty()) {
            installed.append("$HOME/.local/share/applications/" + item);
        }
    }
    QProcess snapProcess;
    snapProcess.start("sh",
                      QStringList() << "-c" << "grep -rl '/var/lib/snapd/desktop/applications/' -e 'jetbrains'");
    snapProcess.waitForFinished(500);
    for (const auto &item :snapProcess.readAllStandardOutput().split('\n')) {
        if (!item.isEmpty()) {
            installed.append(item);
            qInfo() << item;
        }
    }
    QProcess globallyInstalledProcess;
    globallyInstalledProcess.start("sh", QStringList() << "-c" << "ls /usr/share/applications | grep jetbrains");
    globallyInstalledProcess.waitForFinished();
    for (const auto &item :globallyInstalledProcess.readAllStandardOutput().split('\n')) {
        if (!item.isEmpty()) {
            installed.append("/usr/share/applications/" + item);
        }
    }
    std::cout << installed.join('\n').toStdString();
//ls /usr/share/applications | grep jetbrains
}

void JetbrainsRunner::match(Plasma::RunnerContext &context) {
    const QString term = context.query();
    QList<Plasma::QueryMatch> matches;
    Plasma::QueryMatch match(this);
    match.setIconName("kdeapp");
    match.setText("Hello World!");
    matches.append(match);
    context.addMatches(matches);
}

void JetbrainsRunner::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) {
    Q_UNUSED(context)
    Q_UNUSED(match)

    // TODO
}

K_EXPORT_PLASMA_RUNNER(jetbrainsrunner, JetbrainsRunner)

// needed for the QObject subclass declared as part of K_EXPORT_PLASMA_RUNNER
#include "jetbrainsrunner.moc"
