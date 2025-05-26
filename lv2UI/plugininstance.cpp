#include "plugininstance.h"

LV2::Plugin::Instance::Instance() {

}

void LV2::Plugin::Instance::activate()
{
    assert(isValid());
    lilv_instance_activate(_instance);
}

void LV2::Plugin::Instance::deactivate()
{
    assert(isValid());
    lilv_instance_deactivate(_instance);
}
