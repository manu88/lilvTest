#pragma once
#include <stdint.h>
//forward from suil
/// Opaque pointer to a UI widget
typedef void *SuilWidget;

struct NativeWindow
{
    uint32_t windowID;
    int minimumWidth;
    int minimumHeight;
};

NativeWindow getNativeWindowID(SuilWidget widget);
NativeWindow createCalendarWindow();
