#pragma once
#include "plugindescription.h"

class UIManager
{
public:
    UIManager();

    bool createInstanceFor(const LV2::Plugin::Description &desc);
};
