#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H
#include <QString>
#include <lilv/lilv.h>
#include <QList>
#include <QHash>
#include <QVector>
#include "plugindescription.h"
#include "plugininstance.h"

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


            LV2_URID uriMap(const char* uri);
        protected:
            Manager();

        private:

            Description createFromPlugin(const LilvPlugin *p);

            LilvWorld *_world = nullptr;
            LilvNode *_portConnectionOptionalURI;
            LilvNode* _hostType;

            QList<Description> _plugins;
            LV2_URID _hashIndex;
            QHash<QString, LV2_URID> _uriHash;
        };

        Manager& manager();
    }
}
#endif // PLUGINMANAGER_H
