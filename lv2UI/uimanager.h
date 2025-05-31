#pragma once
#include <QList>
#include "plugindescription.h"

namespace LV2 {

namespace UI {

class Manager;
class Instance
{
    friend class LV2::UI::Manager;

private:
    pid_t _pid;
    int toHostFd = -1;
    int fromHostFd = -1;
};

class Manager
{
public:
    bool createInstanceFor(const LV2::Plugin::Description &desc);

private:
    bool waitForHelloMsg(const Instance &instance);
    QList<Instance> _instances;
};
} // namespace UI
} // namespace LV2
