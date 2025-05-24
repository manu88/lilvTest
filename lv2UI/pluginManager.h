#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H
#include <QString>
#include <lilv/lilv.h>
#include <QList>
#include <QVector>
#include "plugindescription.h"

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

        protected:
            Manager();

        private:
            LilvWorld *_world = nullptr;

            Description createFromPlugin(const LilvPlugin *p);

            LilvNode *_portConnectionOptionalURI;
            LilvNode* _hostType;

            QList<Description> _plugins;
        };

        Manager& manager();
    }
}
#endif // PLUGINMANAGER_H
