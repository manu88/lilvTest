#include "uimanager.h"
#include <QProcess>
#include <QUuid>
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
    instance.desc = desc;
    instance.uuid = QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
    instance._pid = pid;
    instance.fromHostFd = hostToAppFDS[0];
    instance.toHostFd = appToHostFDS[1];

    instance.notifier = new QSocketNotifier(instance.fromHostFd, QSocketNotifier::Read);
    connect(instance.notifier, &QSocketNotifier::activated, this, &LV2::UI::Manager::activated);
    _instances.append(instance);

    return true;
}

bool LV2::UI::Manager::deleteInstance(const QString &uuid)
{
    for (auto &instance : _instances) {
        if (instance.uuid == uuid) {
            return sendGoodbye(instance);
        }
    }
    return false;
}

void LV2::UI::Manager::activated(QSocketDescriptor socket, QSocketNotifier::Type type)
{
    for (auto &instance : _instances) {
        if (instance.fromHostFd == (int) socket) {
            if (type == QSocketNotifier::Read) {
                canReadDataFrom(instance);
            } else if (type == QSocketNotifier::Exception) {
                instance.notifier->disconnect();
            }
            return;
        }
    }
}

void LV2::UI::Manager::canReadDataFrom(LV2::UI::Instance &instance)
{
    AppHostMsgFrame msgFrame;
    ssize_t nBytes = read(instance.fromHostFd, &msgFrame, sizeof(AppHostMsgFrame));
    if (nBytes == 0) { // EOF
        qDebug("socket for %s is EoF", instance.uuid.toStdString().c_str());
        instance.notifier->disconnect();
        int removedCount = _instances.removeIf(
            [&instance](const auto &r) { return r.uuid == instance.uuid; });
        qDebug("remove %i instances matching %s", removedCount, instance.uuid.toStdString().c_str());
        emit instancesChanged();
        return;
    }
    qDebug("socket for %s is activated, can read %zi bytes",
           instance.uuid.toStdString().c_str(),
           nBytes);
}

bool LV2::UI::Manager::sendGoodbye(LV2::UI::Instance &instance)
{
    AppHostMsgFrame msgFrame;
    msgFrame.header.msgSize = sizeof(AppHostMsg_Goodbye);
    msgFrame.header.type = AppHostMsgType_Goodbye;
    AppHostMsg_Goodbye msg;
    write(instance.toHostFd, &msgFrame, sizeof(AppHostMsgFrame));
    write(instance.toHostFd, &msg, sizeof(AppHostMsg_Goodbye));
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
