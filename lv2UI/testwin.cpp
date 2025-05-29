#include "testwin.h"
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <gtk/gtkwindow.h>

QWindow *createCalendarWindow()
{
    static bool initializedGTK = []{
        qputenv("GDK_BACKEND", "x11");
        return gtk_init_check(nullptr, nullptr);
    }();
    Q_ASSERT(initializedGTK);

    auto *plug = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(GTK_WIDGET(plug), "delete-event", G_CALLBACK(+[]{
                         return true; // Don't destroy on close
                     }), nullptr);

    auto *calendar = gtk_calendar_new();
    gtk_container_add(GTK_CONTAINER(plug), GTK_WIDGET(calendar));
    gtk_widget_show_all(plug);

    auto *calendarWindow = QWindow::fromWinId(gtk_window_get_xid(GTK_WINDOW(plug)));

    GtkRequisition minimumSize;
    gtk_widget_get_preferred_size(calendar, &minimumSize, NULL);
    calendarWindow->setMinimumSize(QSize(minimumSize.width, minimumSize.height));

    return calendarWindow;
}
