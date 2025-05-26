#pragma once

#include <suil/suil.h>
#include "plugininstance.h"
#include "plugindescription.h"


namespace LV2{
    namespace Plugin
    {
    struct UIInstance
    {
        bool isValid() const { return _uiInstance; }
        SuilInstance *_uiInstance = nullptr;

        void *winHandle = nullptr;
    };
    class UIHost
    {
    public:
        UIHost();
        ~UIHost();

        UIInstance createUIFor(LV2::Plugin::Instance &instance,
                               const LV2::Plugin::Description &desc);

    private:
        SuilHost *_host;
    };
    }
}
