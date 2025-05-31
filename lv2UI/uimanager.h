#pragma once
#include <QList>
#include "plugindescription.h"

namespace LV2 {

namespace UI {

class Manager;
class Instance
{
    friend class LV2::UI::Manager;

public:
    LV2::Plugin::Description desc;
    QString uuid;

private:
    bool _sentHello = false;
    pid_t _pid;
    int toHostFd = -1;
    int fromHostFd = -1;
};

class Manager
{
public:
    bool createInstanceFor(const LV2::Plugin::Description &desc);
    bool deleteInstance(const QString &uuid);

    const QList<Instance> getInstances() const { return _instances; }

private:
    bool waitForHelloMsg(Instance &instance);
    bool sendGoodbye(Instance &instance);
    QList<Instance> _instances;
};
} // namespace UI
} // namespace LV2
