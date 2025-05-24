#ifndef PLUGINS_H
#define PLUGINS_H
#include <QString>
#include <lilv/lilv.h>
#include <QList>
#include <QVector>

namespace LV2{
    namespace Plugin
    {
        struct Description{

            struct UI{
                QString uri;
                bool supported;
            };

            struct Feature{
                bool optional = false;
                QString uri;
            };

            struct Port{
                struct ScalePoint{
                    QString label;
                    float value;
                };

                enum Flow{
                    INPUT,
                    OUTPUT,
                };

                enum Type{
                    AUDIO,
                    CONTROL,
                };
                QString name;
                Flow flow;
                Type type;
                bool optional = false;
                QVector<ScalePoint> scalePoints;
            };

            QString name;
            QString uri;
            QString project;

            QVector<Port> ports;
            QList<Feature> features;
            QVector<UI> uis;
        };

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
#endif // PLUGINS_H
