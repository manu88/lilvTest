#pragma once
#include <lilv/lilv.h>
#include <QString>

namespace LV2{
    namespace Plugin
    {
        class Instance{
        public:
            Instance();

            bool valid()const{
                return _instance != nullptr;
            }

            LilvInstance *_instance;
        };
    }
}
