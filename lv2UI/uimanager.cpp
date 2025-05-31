#include "uimanager.h"
#include <QProcess>
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
    write(instance.toHostFd, "Hello world\n", 12);

    static const int BSIZE = 100;
    static char buf[BSIZE];
    ssize_t nbytes = read(instance.fromHostFd, buf, BSIZE);
    qDebug("received %zi bytes", nbytes);
    _instances.append(instance);
    return true;
}
