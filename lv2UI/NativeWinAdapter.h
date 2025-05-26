#pragma once

//forward from suil
/// Opaque pointer to a UI widget
typedef void *SuilWidget;

struct NativeWindow
{
    void *windowID;
    int minimumWidth;
    int minimumHeight;
};

NativeWindow getNativeWindowID(SuilWidget widget);
NativeWindow createCalendarWindow();
