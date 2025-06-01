#pragma once

#include <QString>
#include <lilv/lilv.h>
#include <QList>
#include <QHash>
#include <QVector>
#include "plugindescription.h"
#include "plugininstance.h"
#include "URIMap.h"

namespace LV2{
    namespace Plugin
    {
        class Manager{
            friend Manager& manager();
        public:
            Manager(Manager const&)         = delete;
            void operator=(Manager const&)  = delete;

            ~Manager();

            QList<Description> getPlugins();
            void refreshPlugins();

            Instance instantiate(const Description &desc);

            LV2_URID uriMap(const QString &uri);
            QString uriUnmap(LV2_URID val);

            static LV2_URID doUriMap(LV2_URID_Map_Handle handle, const char* uri);
        protected:
            Manager();

        private:

            Description createFromPlugin(const LilvPlugin *p);

            LilvWorld *_world = nullptr;
            LilvNode *_portConnectionOptionalURI;
            LilvNode* _hostType;

            QList<Description> _plugins;
            URIMap _uriMap;
        };

        Manager& manager();
    }
}
