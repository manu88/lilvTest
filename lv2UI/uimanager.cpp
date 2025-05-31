#include "uimanager.h"
#include <QProcess>
#include <QUuid>
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
            instance._shouldBeDeleted = true;
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
    AppHostHeader msgHeader;
    ssize_t nBytes = read(instance.fromHostFd, &msgHeader, sizeof(AppHostHeader));
    if (nBytes == 0) { // EOF
        qDebug("socket for %s is EoF", instance.uuid.toStdString().c_str());
        instance.notifier->disconnect();
        int removedCount = _instances.removeIf(
            [&instance](const auto &r) { return r.uuid == instance.uuid; });
        qDebug("remove %i instances matching %s", removedCount, instance.uuid.toStdString().c_str());
        emit instancesChanged();
        return;
    }
    if (nBytes == sizeof(AppHostHeader)) {
        qDebug("got header, type %i size %i", msgHeader.type, msgHeader.msgSize);
        char buf[HOST_PROTOCOL_MAX_MSG_SIZE];
        nBytes = read(instance.fromHostFd, buf, msgHeader.msgSize);
        if (nBytes == msgHeader.msgSize) {
            qDebug("got complete message type %i size %i", msgHeader.type, msgHeader.msgSize);
            onMessageFrom(instance, &msgHeader, (const void *) buf);
        }
    }
}

void LV2::UI::Manager::onMessageFrom(LV2::UI::Instance &instance,
                                     const AppHostHeader *header,
                                     const void *data)
{
    switch (header->type) {
    case AppHostMsgType_Hello: {
        const AppHostMsg_Hello *msgHello = (const AppHostMsg_Hello *) data;
        qDebug("host protocol = %i, App is %i\n", msgHello->protocolVersion, HOST_PROTOCOL_VERSION);
        instance._sentHello = true;
        break;
    }
    default:
        break;
    }
}

bool LV2::UI::Manager::sendGoodbye(LV2::UI::Instance &instance)
{
    AppHostHeader header;
    header.msgSize = sizeof(AppHostMsg_Goodbye);
    header.type = AppHostMsgType_Goodbye;
    AppHostMsg_Goodbye msg;
    write(instance.toHostFd, &header, sizeof(AppHostHeader));
    write(instance.toHostFd, &msg, sizeof(AppHostMsg_Goodbye));
    return true;
}
