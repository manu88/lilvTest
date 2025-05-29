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

UIManager::UIManager() {}

bool UIManager::createInstanceFor(const LV2::Plugin::Description &desc){

    auto uiProcess = new QProcess();
    QStringList args;
    args << desc.uri;
    uiProcess->start(TESTUI_PATH, args);

    return false;
}
