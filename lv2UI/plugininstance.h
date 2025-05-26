#pragma once
#include <lilv/lilv.h>
#include <QString>

namespace LV2{
    namespace Plugin
    {
        class Instance{
        public:
            Instance();

            bool isValid() const { return _instance != nullptr; }

            void activate();
            void deactivate();

            LilvInstance *_instance;
        };
    }
}
