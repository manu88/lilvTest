#ifndef UIHOST_H
#define UIHOST_H

#include <suil/suil.h>
#include "plugininstance.h"
#include "plugindescription.h"


namespace LV2{
    namespace Plugin
    {
        class UIHost
        {
        public:
            UIHost();
            ~UIHost();

            bool createUIFor(LV2::Plugin::Instance &instance, const LV2::Plugin::Description &desc);

        private:
            SuilHost* _host;
        };
    }
}
#endif // UIHOST_H
