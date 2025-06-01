#pragma once
#include <QString>
#include <QVector>
#include <lilv/lilv.h>

namespace LV2{
    namespace Plugin{
        struct Description{
            friend class Manager;
            friend class UIHost;

            struct UI{
                friend class Manager;
                friend class UIHost;
                QString uri;
                QString uiType;
                bool isNative = false;

            private:
                const LilvUI* _ptr;
                const LilvNode* _uriNode;
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
            QString bundleUri;
            QString project;

            QVector<Port> ports;
            QList<Feature> features;
            QVector<UI> uis;

        private:
            const LilvPlugin *_ptr;
        };
    }
}
