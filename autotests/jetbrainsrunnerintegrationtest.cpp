#include <KRunner/AbstractRunnerTest>
#include <KShell>
#include <KSycoca>
#include <QDir>
#include <QStandardPaths>
#include <QTest>

using namespace KRunner;

class JetbrainsRunnerIntegrationTest : public AbstractRunnerTest
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
        using QSP = QStandardPaths;
        QSP::setTestModeEnabled(true);
        const QString dataLocation = QSP::writableLocation(QStandardPaths::ApplicationsLocation);
        QDir(dataLocation).removeRecursively();
        const QString configLocation = QSP::writableLocation(QStandardPaths::GenericConfigLocation) + "/JetBrains/PhpStorm2022.2/options/";
        QDir(configLocation).removeRecursively();

        QVERIFY(QDir().mkpath(dataLocation));
        QVERIFY(QDir().mkpath(configLocation));

        QVERIFY(QDir::home().mkpath(".qtest/PhpstormProjects/My Dummy Project"));
        QVERIFY(QDir::home().mkpath(".qtest/PhpstormProjects/dummy"));

        QFile::copy(QFINDTESTDATA("data/jetbrains-phpstorm.desktop"), dataLocation + "/jetbrains-phpstorm.desktop");
        QFile::copy(QFINDTESTDATA("data/recentProjects.xml"), configLocation + "recentProjects.xml");
        KSycoca::self()->ensureCacheValid();

        initProperties();
    }
    void testProjectWithSpace()
    {
        launchQuery("phpstorm");
        const QList<QueryMatch> matches = manager->matches();
        QCOMPARE(matches.size(), 2);

        QCOMPARE(matches.at(0).text(), "PhpStorm launch dummy");
        const QString expectedDummyExec =
            "env XMODIFIERS=phpstorm.sh \"/home/user/apps/PhpStorm-222.4345.15/bin/phpstorm.sh\" " + QDir::homePath() + "/.qtest/PhpstormProjects/dummy";
        QCOMPARE(matches.at(0).data().toString(), expectedDummyExec);
        QCOMPARE(matches.at(1).text(), "PhpStorm launch My Dummy Project");
        const QString expectedExecForProjectWithSpaceInPath = "env XMODIFIERS=phpstorm.sh \"/home/user/apps/PhpStorm-222.4345.15/bin/phpstorm.sh\" "
            + KShell::quoteArg(QDir::homePath() + "/.qtest/PhpstormProjects/My Dummy Project");
        QCOMPARE(matches.at(1).data().toString(), expectedExecForProjectWithSpaceInPath);
    }
};

QTEST_MAIN(JetbrainsRunnerIntegrationTest)

#include "jetbrainsrunnerintegrationtest.moc"
