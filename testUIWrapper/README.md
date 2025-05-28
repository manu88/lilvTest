# Objective:
Write a wrapper that will display LV2 UI's:
the [LV2 UI specs](http://lv2plug.in/ns/extensions/ui) lists different UI classes, such as 
Gtk3UI, Qt4UI, X11, etc.

Problem is: If creating a Qt host it will be impossible/very hard to embed the UI directly the app itself for at least 2 reasons:
1. multiple versions of UI libs can not be used in the same process
2. Most (recent) Linux distros use Wayland compositor, so we can't use X11 directly


# Plan:
On a Linux/Wayland distro, with the host written in Qt6:
Use the lowest denominator possible for plugins -- X11 -- and use `xvf` or [this](https://lists.freedesktop.org/archives/wayland-devel/2023-November/043329.html) as a wrapper for all x11-based LV2 UI classes.