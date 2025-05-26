#include "NativeWinAdapter.h"
#include <assert.h>
#include <gtk/gtk.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtkwidget.h>
#include <stdlib.h>

NativeWindow getNativeWindowID(SuilWidget widget)
{
    GtkWidget *window = (GtkWidget *) widget;
    //gtk_widget_show(window);

    GtkRequisition minimumSize;
    gtk_widget_size_request(window, &minimumSize);

    auto *calendar = gtk_calendar_new();
    gtk_widget_show_all(calendar);
    auto *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_widget_show_all(win);
    void *nativeID = GDK_WINDOWING_QUARTZ(win);

    return {
        .windowID = nativeID,
        .minimumWidth = minimumSize.width,
        .minimumHeight = minimumSize.height,
    };
}

NativeWindow createCalendarWindow()
{
    static bool initializedGTK = [] {
        putenv("GDK_BACKEND=x11");
        return gtk_init_check(nullptr, nullptr);
    }();
    assert(initializedGTK);
    auto *plug = gtk_plug_new(0);

    auto *button_box = gtk_button_new_with_label("hello");
    gtk_container_add(GTK_CONTAINER(plug), button_box);

    gtk_widget_show_all(plug);

    GtkRequisition minimumSize;

    return {
        .windowID = gtk_plug_get_id(GTK_PLUG(plug)),
        .minimumWidth = minimumSize.width,
        .minimumHeight = minimumSize.height,
    };
}
