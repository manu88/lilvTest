#include "uimanager.h"
#include <QProcess>
#include "HostProtocol.h"
#include <unistd.h> // fork
// not very universal and multi-users right now :)
#ifdef Q_OS_LINUX
#define TESTUI_PATH "/home/vboxuser/dev/testLV2/UIHost/UIHost"
#elif defined(Q_OS_MACOS)
#define TESTUI_PATH "/Users/manueldeneu/Documents/dev/lilvTest/UIHost/UIHost"
#else
#error "unknown platform"
#endif

bool LV2::UI::Manager::createInstanceFor(const LV2::Plugin::Description &desc)
{
    int err = 0;
    int appToHostFDS[2];
    err = pipe(appToHostFDS);
    if (err == -1) {
        err = errno;
        perror("pipe appToHost");
        qDebug("appToHost pipe error: %i\n", err);
        return false;
    }

    int hostToAppFDS[2];
    err = pipe(hostToAppFDS);
    if (err == -1) {
        err = errno;
        perror("pipe hostToApp");
        qDebug("hostToApp pipe error: %i\n", err);
        return false;
    }

    pid_t pid = fork();
    if (pid == -1) {
        assert(0);
    } else if (pid == 0) {
        // child
        close(appToHostFDS[1]);
        close(hostToAppFDS[0]);
        const auto pluginURI = desc.uri.toStdString();
        const auto appToHostPipeFD = std::to_string(appToHostFDS[0]);
        const auto hostToAppPipeFD = std::to_string(hostToAppFDS[1]);
        const char *args[] = {"UIHost",
                              pluginURI.c_str(),
                              appToHostPipeFD.c_str(),
                              hostToAppPipeFD.c_str(),
                              NULL};
        if (execv(TESTUI_PATH, const_cast<char *const *>(args)) == -1) {
            err = errno;
            perror("execv");
            qDebug("execv error: %i\n", err);
            return false;
        }
    }
    close(appToHostFDS[0]);
    close(hostToAppFDS[1]);
    qDebug("child process pid %i", pid);

    auto instance = LV2::UI::Instance();
    instance._pid = pid;
    instance.fromHostFd = hostToAppFDS[0];
    instance.toHostFd = appToHostFDS[1];
    _instances.append(instance);

    waitForHelloMsg(instance);
    return true;
}

static const int BSIZE = 100;
static char buf[BSIZE];

bool LV2::UI::Manager::waitForHelloMsg(LV2::UI::Instance &instance)
{
    ssize_t nbytes = read(instance.fromHostFd, buf, sizeof(AppHostMsgFrame));
    qDebug("received %zi bytes", nbytes);
    const AppHostMsgFrame *recvFrame = (const AppHostMsgFrame *) &buf;
    qDebug("received msg size = %i type=%i", recvFrame->header.msgSize, recvFrame->header.type);
    if (recvFrame->header.type == AppHostMsgType_Hello) {
        ssize_t nbytes = read(instance.fromHostFd, buf, recvFrame->header.msgSize);
        qDebug("received %zi bytes", nbytes);
        const AppHostMsg_Hello *helloMsg = (const AppHostMsg_Hello *) &buf;
        qDebug("host protocol = %i, App is %i\n", helloMsg->protocolVersion, HOST_PROTOCOL_VERSION);
        instance._sentHello = true;
        return true;
    }
    return false;
}
