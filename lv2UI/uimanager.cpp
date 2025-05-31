#include "uimanager.h"
#include <QProcess>

// not very universal and multi-users right now :)
#ifdef Q_OS_LINUX
#define TESTUI_PATH "/home/vboxuser/dev/testLV2/nativeTest/testUI"
#elif defined(Q_OS_MACOS)
#define TESTUI_PATH "/Users/manueldeneu/Documents/dev/lilvTest/nativeTest/testUI"
#else
#error "unknown platform"
#endif

LV2::UI::Manager::Manager() {}

bool LV2::UI::Manager::createInstanceFor(const LV2::Plugin::Description &desc)
{
    auto uiProcess = LV2::UI::Instance();
    uiProcess._process = new QProcess(this);

    connect(uiProcess._process, &QProcess::finished, this, &LV2::UI::Manager::finished);
    connect(uiProcess._process, &QProcess::errorOccurred, this, &LV2::UI::Manager::errorOccurred);

    QStringList args;
    args << desc.uri;
    uiProcess._process->start(TESTUI_PATH, args);
    //_instances.append(uiProcess);
    return false;
}

void LV2::UI::Manager::finished(int exitCode,
                                QProcess::ExitStatus exitStatus /*= QProcess::NormalExit*/)
{
    qDebug("Process %s code %i",
           (exitStatus == QProcess::NormalExit ? "finished" : "crashed"),
           exitCode);
}

void LV2::UI::Manager::errorOccurred(QProcess::ProcessError error)
{
    qDebug("errorOccurred on process");
}
