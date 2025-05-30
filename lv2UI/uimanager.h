#pragma once
#include <QProcess>
#include "plugindescription.h"

class UIManager : public QObject
{
    Q_OBJECT
public:
    UIManager();

    bool createInstanceFor(const LV2::Plugin::Description &desc);

private:
private Q_SLOTS:
    void finished(int exitCode, QProcess::ExitStatus exitStatus = QProcess::NormalExit);
};
