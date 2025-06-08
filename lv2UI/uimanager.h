#pragma once
#include <QList>
#include <QObject>
#include <QSocketNotifier>
#include "HostProtocol.h"
#include "plugindescription.h"
#include <suil/suil.h>

namespace LV2 {

namespace UI {

class Manager;

class Instance
{
public:
    enum class Type : int {
        Native,
        Foreign,
    };
    virtual ~Instance() {}
    LV2::Plugin::Description desc;
    QString uuid;
    Type type;

protected:
    Instance(Type type)
        : type(type)
    {}
};

class ForeignInstance : public Instance
{
    friend class LV2::UI::Manager;

public:
    ForeignInstance()
        : Instance(Type::Foreign)
    {}

private:
    bool _sentHello = false;
    pid_t _pid;
    int toHostFd = -1;
    int fromHostFd = -1;
    QSocketNotifier *notifier;
};

class NativeInstance : public Instance
{
public:
    NativeInstance()
        : Instance(Type::Native)
    {}
};

class Manager : public QObject
{
    Q_OBJECT
public:
    Manager();
    bool createInstanceFor(const LV2::Plugin::Description &desc);
    bool deleteInstance(const QString &uuid);
    void cleanup();

    const QList<Instance *> getInstances() const { return _instances; }

signals:
    void instancesChanged();

private slots:
    void activated(QSocketDescriptor socket, QSocketNotifier::Type type);

private:
    Instance *createUIHostInstanceFor(const LV2::Plugin::Description &desc);
    Instance *createNativeInstanceFor(const LV2::Plugin::Description &desc,
                                      const LV2::Plugin::Description::UI &ui);
    void onMessageFrom(ForeignInstance *instance, const AppHostHeader *header, const void *data);
    void canReadDataFrom(ForeignInstance *instance);

    bool deleteNativeInstance(NativeInstance *instance);
    bool sendGoodbye(ForeignInstance *instance);
    QList<Instance *> _instances;

    SuilHost *_host;
};
} // namespace UI
} // namespace LV2
