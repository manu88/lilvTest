#include "uimanager.h"
#include <QProcess>

UIManager::UIManager() {}

bool UIManager::createInstanceFor(const LV2::Plugin::Description &desc){

    auto uiProcess = new QProcess();
    QStringList args;
    args << desc.uri;
    uiProcess->start("/home/vboxuser/dev/testLV2/nativeTest/testUI", args);

    return false;
}
