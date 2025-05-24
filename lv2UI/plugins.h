#ifndef PLUGINS_H
#define PLUGINS_H
#include <QString>
#include <lilv/lilv.h>
#include <QList>
#include <QVector>

namespace LV2{


class Plugin
{
public:

    struct Description{
        struct Feature{
            bool optional = false;
            QString uri;
        };

        struct Port{
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
        };

        QString name;
        QString uri;
        QString project;

        QVector<Port> ports;
        QList<Feature> features;
    };

    class Manager{
        friend class Plugin;
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

        QList<Description> _plugins;
    };

    static Manager& manager(){
        static Manager m;
        return m;
    }

    Plugin();
    ~Plugin();

private:


};

}
#endif // PLUGINS_H
