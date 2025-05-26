#include "uihost.h"
#include <QDebug>
#include <QWindow>
#include "NativeWinAdapter.h"
#include "pluginManager.h"
#include <lilv/lilv.h>
#include <lv2/ui/ui.h>
#include <serd/serd.h>

static void _SuilPortWriteFunc(SuilController controller,
                               uint32_t port_index,
                               uint32_t buffer_size,
                               uint32_t protocol,
                               void const *buffer);
static uint32_t _SuilPortIndexFunc(SuilController controller, const char*    port_symbol);
static uint32_t _SuilPortSubscribeFunc(SuilController controller, uint32_t port_index, uint32_t protocol, const LV2_Feature* const* features);
static uint32_t _SuilPortUnsubscribeFunc( SuilController controller, uint32_t port_index, uint32_t protocol, const LV2_Feature* const* features);

static void _SuilTouchFunc(SuilController controller, uint32_t port_index, bool grabbed)
{
    qDebug("_SuilTouchFunc");
}

LV2::Plugin::UIHost::UIHost()
{
    qDebug("Init UI host");

    suil_init(0, NULL, SUIL_ARG_NONE);

    _host = suil_host_new(_SuilPortWriteFunc,
                                 _SuilPortIndexFunc,
                                 _SuilPortSubscribeFunc,
                                 _SuilPortUnsubscribeFunc);

    suil_host_set_touch_func(_host, _SuilTouchFunc);
}

LV2::Plugin::UIHost::~UIHost()
{
    suil_host_free(_host);
}

static void _SuilPortWriteFunc(SuilController controller, uint32_t port_index, uint32_t buffer_size, uint32_t protocol, void const* buffer){
    const char *protocolURI = LV2::Plugin::manager().uriUnmap(protocol);
    qDebug("_SuilPortWriteFunc call on port %u protocol %u '%s'", port_index, protocol, protocolURI);
}

static uint32_t _SuilPortIndexFunc( SuilController controller, const char*    port_symbol){
    return 0;
}

static uint32_t _SuilPortSubscribeFunc(SuilController controller, uint32_t port_index, uint32_t protocol, const LV2_Feature* const* features){
    return 0;
}

static uint32_t _SuilPortUnsubscribeFunc( SuilController controller, uint32_t port_index, uint32_t protocol, const LV2_Feature* const* features){
    return 0;
}

LV2::Plugin::UIInstance LV2::Plugin::UIHost::createUIFor(LV2::Plugin::Instance &instance,
                                                         const LV2::Plugin::Description &desc)
{
    qDebug("create UI for instance %s", desc.uri.toStdString().c_str());

    const LilvUI *uiptr = desc.uis[0]._ptr;

    const LilvNode *binaryURINode = lilv_ui_get_binary_uri(uiptr);
    const LilvNode *bundleURINode = lilv_ui_get_bundle_uri(uiptr);
    assert(lilv_node_is_uri(binaryURINode));
    assert(lilv_node_is_uri(bundleURINode));

    const char *binary_uri = lilv_node_as_uri(binaryURINode);
    const char *bundle_uri = lilv_node_as_uri(bundleURINode);
    qDebug("binaryURI '%s'", binary_uri);
    qDebug("bundleURI '%s'", bundle_uri);

    const char *bundle_path = (const char *) serd_file_uri_parse((const uint8_t *) bundle_uri, NULL);
    const char *binary_path = (const char *) serd_file_uri_parse((const uint8_t *) binary_uri, NULL);

    const char *uiTypeUri = LV2_UI__GtkUI;
    const char *containerTypeUri = LV2_UI__GtkUI; //LV2_UI__GtkUI;
    qDebug("bundle_path='%s' binary_path='%s'", bundle_path, binary_path);

    LV2_URID_Map mapHandle;
    mapHandle.map = LV2::Plugin::Manager::doUriMap;
    mapHandle.handle = &LV2::Plugin::manager();
    LV2_Feature feat;
    feat.URI = LV2_URID__map;
    feat.data = &mapHandle;
    LV2_Feature *features[2] = {&feat, NULL};

    SuilInstance *uiInstance = suil_instance_new(_host,
                                                 this,
                                                 containerTypeUri,
                                                 lilv_node_as_uri(lilv_plugin_get_uri(desc._ptr)),
                                                 lilv_node_as_uri(lilv_ui_get_uri(uiptr)),
                                                 uiTypeUri,
                                                 bundle_path,
                                                 binary_path,
                                                 features);
    LV2::Plugin::UIInstance ret;
    if (uiInstance) {
        ret._uiInstance = uiInstance;
        SuilWidget widget = suil_instance_get_widget(uiInstance);
        auto nativeWin = createCalendarWindow();
        qDebug("native win size %i %i", nativeWin.minimumWidth, nativeWin.minimumHeight);
        ret.winHandle = nativeWin.windowID;
    }
    return ret;
}
