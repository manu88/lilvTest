#pragma once
#include <QString>
#include <QVector>

namespace LV2{
    namespace Plugin{
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
    }
}
