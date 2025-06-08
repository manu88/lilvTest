#include "uimanager.h"
#include <QProcess>
#include <QUuid>
#include "pluginManager.h"
#include <lv2/atom/atom.h>
#include <lv2/atom/util.h>
#include <serd/serd.h>
#include <unistd.h> // fork

// not very universal and multi-users right now :)
#ifdef Q_OS_LINUX
#define TESTUI_PATH "/home/vboxuser/dev/testLV2/UIHost/UIHost"
#elif defined(Q_OS_MACOS)
#define TESTUI_PATH "/Users/manueldeneu/Documents/dev/lilvTest/UIHost/UIHost"
#else
#error "unknown platform"
#endif

static void _SuilPortWriteFunc(SuilController controller,
                               uint32_t port_index,
                               uint32_t buffer_size,
                               uint32_t protocol,
                               void const *buffer)
{}

static uint32_t _SuilPortIndexFunc(SuilController controller, const char *port_symbol)
{
    return 0;
}

static uint32_t _SuilPortSubscribeFunc(SuilController controller,
                                       uint32_t port_index,
                                       uint32_t protocol,
                                       const LV2_Feature *const *features)
{
    return 0;
}

static uint32_t _SuilPortUnsubscribeFunc(SuilController controller,
                                         uint32_t port_index,
                                         uint32_t protocol,
                                         const LV2_Feature *const *features)
{
    return 0;
}

LV2::UI::Manager::Manager()
{
    _host = suil_host_new(_SuilPortWriteFunc,
                          _SuilPortIndexFunc,
                          _SuilPortSubscribeFunc,
                          _SuilPortUnsubscribeFunc);
}

bool LV2::UI::Manager::createInstanceFor(const LV2::Plugin::Description &desc)
{
    if (!desc.hasUI()) {
        qDebug("plugin %s has no UI", desc.name.toStdString().c_str());
        return false;
    }

    Instance *instance = nullptr;
    for (const auto &ui : desc.uis) {
        if (ui.isNative) {
            instance = createNativeInstanceFor(desc, ui);
            if (instance) {
                break;
            }
        }
    }
    if (instance == nullptr) {
        instance = createUIHostInstanceFor(desc);
    }
    if (instance == nullptr) {
        return false;
    }
    instance->desc = desc;
    instance->uuid = QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
    _instances.append(instance);
    return true;
}

LV2::UI::Instance *LV2::UI::Manager::createNativeInstanceFor(const LV2::Plugin::Description &desc,
                                                             const LV2::Plugin::Description::UI &ui)
{
    qDebug("create native UI instance for %s using %s",
           desc.name.toStdString().c_str(),
           ui.uiType.toStdString().c_str());
#if 0
    const char *bundle_uri = lilv_node_as_uri(lilv_ui_get_bundle_uri(ui._ptr));
    const char *binary_uri = lilv_node_as_uri(lilv_ui_get_binary_uri(ui._ptr));
    const char *bundle_path = (const char *) serd_file_uri_parse((const uint8_t *) bundle_uri, NULL);
    const char *binary_path = (const char *) serd_file_uri_parse((const uint8_t *) binary_uri, NULL);
    SuilInstance *uiInstance = suil_instance_new(_host,
                                                 this,
                                                 LV2_UI__GtkUI,
                                                 lilv_node_as_uri(lilv_plugin_get_uri(plug)),
                                                 lilv_node_as_uri(lilv_ui_get_uri(ui)),
                                                 LV2_UI__GtkUI,
                                                 bundle_path,
                                                 binary_path,
                                                 features);

    serd_free((void *) bundle_path);
    serd_free((void *) binary_path);
#endif
    auto *instance = new NativeInstance();
    return instance;
}

LV2::UI::Instance *LV2::UI::Manager::createUIHostInstanceFor(const LV2::Plugin::Description &desc)
{
    qDebug("create UIHost UI instance for %s using ", desc.name.toStdString().c_str());
    int err = 0;
    int appToHostFDS[2];
    err = pipe(appToHostFDS);
    if (err == -1) {
        err = errno;
        perror("pipe appToHost");
        qDebug("appToHost pipe error: %i\n", err);
        return nullptr;
    }

    int hostToAppFDS[2];
    err = pipe(hostToAppFDS);
    if (err == -1) {
        err = errno;
        perror("pipe hostToApp");
        qDebug("hostToApp pipe error: %i\n", err);
        return nullptr;
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
            return nullptr;
        }
    }
    close(appToHostFDS[0]);
    close(hostToAppFDS[1]);
    qDebug("child process pid %i", pid);

    auto instance = new LV2::UI::ForeignInstance();
    instance->_pid = pid;
    instance->fromHostFd = hostToAppFDS[0];
    instance->toHostFd = appToHostFDS[1];

    instance->notifier = new QSocketNotifier(instance->fromHostFd, QSocketNotifier::Read);
    connect(instance->notifier, &QSocketNotifier::activated, this, &LV2::UI::Manager::activated);
    return instance;
}

bool LV2::UI::Manager::deleteInstance(const QString &uuid)
{
    for (auto instance : std::as_const(_instances)) {
        if (instance->uuid == uuid) {
            if (instance->type == Instance::Type::Native) {
                return deleteNativeInstance(dynamic_cast<LV2::UI::NativeInstance *>(instance));
            } else {
                // will be removed from _instances when EOF is received on socket
                return sendGoodbye(dynamic_cast<ForeignInstance *>(instance));
            }
        }
    }
    return false;
}

bool LV2::UI::Manager::deleteNativeInstance(LV2::UI::NativeInstance *instance)
{
    bool ret = _instances.removeIf([&instance](const auto &r) { return r->uuid == instance->uuid; })
               == 1;

    emit instancesChanged();
    return ret;
}

void LV2::UI::Manager::cleanup()
{
    qDebug("UI::Manager::cleanup");
    for (auto instance : std::as_const(_instances)) {
        if (instance->type == Instance::Type::Native) {
            deleteNativeInstance(dynamic_cast<LV2::UI::NativeInstance *>(instance));
        } else {
            sendGoodbye(dynamic_cast<ForeignInstance *>(instance));
        }
    }
}

void LV2::UI::Manager::activated(QSocketDescriptor socket, QSocketNotifier::Type type)
{
    for (auto theInstance : std::as_const(_instances)) {
        if (theInstance->type != Instance::Type::Foreign) {
            continue;
        }
        auto *instance = dynamic_cast<ForeignInstance *>(theInstance);
        if (instance->fromHostFd == (int) socket) {
            if (type == QSocketNotifier::Read) {
                canReadDataFrom(instance);
            } else if (type == QSocketNotifier::Exception) {
                instance->notifier->disconnect();
            }
            return;
        }
    }
}

void LV2::UI::Manager::canReadDataFrom(LV2::UI::ForeignInstance *instance)
{
    AppHostHeader msgHeader;
    ssize_t nBytes = read(instance->fromHostFd, &msgHeader, sizeof(AppHostHeader));
    if (nBytes == 0) { // EOF
        qDebug("socket for %s is EoF", instance->uuid.toStdString().c_str());
        instance->notifier->disconnect();
        int removedCount = _instances.removeIf(
            [&instance](const auto &r) { return r->uuid == instance->uuid; });
        qDebug("remove %i instances matching %s",
               removedCount,
               instance->uuid.toStdString().c_str());
        emit instancesChanged();
        return;
    }
    if (nBytes == sizeof(AppHostHeader)) {
        char buf[HOST_PROTOCOL_MAX_MSG_SIZE];
        nBytes = read(instance->fromHostFd, buf, msgHeader.msgSize);
        if (nBytes == msgHeader.msgSize) {
            onMessageFrom(instance, &msgHeader, (void *) buf);
        }
    }
}

void LV2::UI::Manager::onMessageFrom(LV2::UI::ForeignInstance *instance,
                                     const AppHostHeader *header,
                                     const void *data)
{
    switch (header->type) {
    case AppHostMsgType_Hello: {
        const AppHostMsg_Hello *msgHello = (const AppHostMsg_Hello *) data;
        instance->_sentHello = true;
        break;
    }
    case AppHostMsgType_URIDMapRequest: {
        AppHostMsg_URIDMapRequest *mapRequest = (AppHostMsg_URIDMapRequest *) data;
        uint32_t urid = LV2::Plugin::manager().uriMap(mapRequest->uri);

        AppHostHeader headerReply;
        headerReply.type = AppHostMsgType_URIDMapReply;
        headerReply.msgSize = sizeof(AppHostMsg_URIDMapReply);
        write(instance->toHostFd, &headerReply, sizeof(AppHostHeader));
        AppHostMsg_URIDMapReply reply;
        reply.urid = urid;
        write(instance->toHostFd, &reply, sizeof(AppHostMsg_URIDMapReply));
        break;
    }
    case AppHostMsgType_URIDUnMapRequest: {
        const AppHostMsg_URIDUnMapRequest *unmapRequest = (AppHostMsg_URIDUnMapRequest *) data;
        QString uri = LV2::Plugin::manager().uriUnmap(unmapRequest->urid);
        AppHostHeader headerReply;
        headerReply.type = AppHostMsgType_URIDUnMapReply;
        headerReply.msgSize = uri.length() + 1;
        write(instance->toHostFd, &headerReply, sizeof(AppHostHeader));
        write(instance->toHostFd, uri.toStdString().c_str(), headerReply.msgSize);
        break;
    }
    case AppHostMsgType_PortWriteRequest: {
        const AppHostMsg_PortWriteRequest *portWriteReq = (const AppHostMsg_PortWriteRequest *) data;
        const char *buffer = (const char *) data + sizeof(AppHostMsg_PortWriteRequest);
        qDebug("port write request on port %i protocol %i size %i\n",
               portWriteReq->portIndex,
               portWriteReq->protocol,
               portWriteReq->bufferSize);

        const QString protocolName = LV2::Plugin::manager().uriUnmap(portWriteReq->protocol);
        qDebug("_suilPortWriteFunc on protocol %u '%s' port index %u",
               portWriteReq->protocol,
               protocolName.toStdString().c_str(),
               portWriteReq->portIndex);

        if (protocolName == LV2_ATOM__eventTransfer) {
            qDebug("\tEvent transfer buffer size %u", portWriteReq->bufferSize);

            const LV2_Atom_Object *obj = (const LV2_Atom_Object *) buffer;
            LV2_ATOM_OBJECT_FOREACH(obj, iter)
            {
                const QString typeURI = LV2::Plugin::manager().uriUnmap(iter->value.type);

                const QString keyURI = LV2::Plugin::manager().uriUnmap(iter->key);
                qDebug("Key %i '%s' type %s",
                       iter->key,
                       keyURI.toStdString().c_str(),
                       typeURI.toStdString().c_str());
                if (keyURI == "http://lv2plug.in/plugins/eg-scope#ui-spp") {
                    const LV2_Atom_Int *val = (const LV2_Atom_Int *) &iter->value;
                    qDebug("val %i", val->body);
                } else if (keyURI == "http://lv2plug.in/plugins/eg-scope#ui-amp") {
                    const LV2_Atom_Float *val = (const LV2_Atom_Float *) &iter->value;
                    qDebug("val %f", val->body);
                }
            }
        }

        break;
    }
    default:
        break;
    }
}

bool LV2::UI::Manager::sendGoodbye(LV2::UI::ForeignInstance *instance)
{
    qDebug("send goodbye to %s", instance->uuid.toStdString().c_str());
    AppHostHeader header;
    header.msgSize = sizeof(AppHostMsg_Goodbye);
    header.type = AppHostMsgType_Goodbye;
    AppHostMsg_Goodbye msg;
    write(instance->toHostFd, &header, sizeof(AppHostHeader));
    write(instance->toHostFd, &msg, sizeof(AppHostMsg_Goodbye));
    return true;
}
