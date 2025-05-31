# Objective:
Display any LV2 UI plugin in a Qt6 application.

since LV2 plugins can use different UI libraries (gtk2, gtk3, x11, cocoa, etc.), it means some wrapping must occur.

This test project consists in two sub-projects:
1. the main application, written in Qt6, uses `liblilv` to scan host plugins, show information and instantiate them. then
2. a UIHost process is spawn and will take care of handling the display of the plugin UI.

Note that in the case where the plugin's UI matches the host configuration, the main application should directly create the UI instance. This will also be the case if `suil` provides a wrapper widget to bridge the plugin UI type to the host type. 


## things to install

```bash
sudo apt install liblilv-dev libsuil-dev
```

## things to read
[lilv docs](https://drobilla.net/docs/lilv/)

[list of LV2 plugins/projects](https://lv2plug.in/pages/projects.html)

[Where are all the lv2-related files?](https://lv2plug.in/pages/filesystem-hierarchy-standard.html)

 