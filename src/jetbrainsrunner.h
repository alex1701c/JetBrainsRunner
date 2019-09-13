#ifndef JETBRAINSRUNNER_H
#define JETBRAINSRUNNER_H

//#define LOG_INSTALLED

#include <KRunner/AbstractRunner>
#include "JetbrainsApplication.h"

class JetbrainsRunner : public Plasma::AbstractRunner {
Q_OBJECT

public:
    JetbrainsRunner(QObject *parent, const QVariantList &args);

    ~JetbrainsRunner() override;

    QList<JetbrainsApplication> installed;

    KConfigGroup config;

    QList<Plasma::QueryMatch> addAppNameMatches(const QString &term);

    QList<Plasma::QueryMatch> addProjectNameMatches(const QString &term);

protected Q_SLOTS:

    void init() override;

public: // Plasma::AbstractRunner API
    void match(Plasma::RunnerContext &context) override;

    void run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) override;
};

#endif
