#pragma once
#include <QList>
#include <QProcess>
#include "plugindescription.h"

namespace LV2 {

namespace UI {

class Manager;
class Instance : public QObject
{
    Q_OBJECT
    friend class LV2::UI::Manager;

private:
    QProcess *_process;
};

class Manager : public QObject
{
    Q_OBJECT
public:
    Manager();

    bool createInstanceFor(const LV2::Plugin::Description &desc);

private:
    QList<Instance> _instances;

private Q_SLOTS:
    void finished(int exitCode, QProcess::ExitStatus exitStatus = QProcess::NormalExit);
    void errorOccurred(QProcess::ProcessError error);
};
} // namespace UI
} // namespace LV2
