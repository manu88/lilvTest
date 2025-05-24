#include "uihost.h"
#include <lilv/lilv.h>
#include <QDebug>


static void _SuilPortWriteFunc(SuilController controller, uint32_t port_index, uint32_t buffer_size, uint32_t protocol, void const* buffer);
static uint32_t _SuilPortIndexFunc(SuilController controller, const char*    port_symbol);
static uint32_t _SuilPortSubscribeFunc(SuilController controller, uint32_t port_index, uint32_t protocol, const LV2_Feature* const* features);
static uint32_t _SuilPortUnsubscribeFunc( SuilController controller, uint32_t port_index, uint32_t protocol, const LV2_Feature* const* features);


UIHost::UIHost() {
    qDebug("Init UI host");


    suil_init(0, NULL, SUIL_ARG_NONE);

    _host = suil_host_new(_SuilPortWriteFunc,
                                 _SuilPortIndexFunc,
                                 _SuilPortSubscribeFunc,
                                 _SuilPortUnsubscribeFunc);
}

UIHost::~UIHost(){
    suil_host_free(_host);
}


static void _SuilPortWriteFunc(SuilController controller, uint32_t port_index, uint32_t buffer_size, uint32_t protocol, void const* buffer){

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
