#include "mainwindow.h"

#include <QApplication>

extern "C" void gdk_init(int *argc, char ***argv);
extern "C" void gtk_init(int *argc, char ***argv);

int main(int argc, char *argv[])
{
    gtk_init(0, NULL);
    gdk_init(0, NULL);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
