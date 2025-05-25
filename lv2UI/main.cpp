#include "mainwindow.h"

#include <QApplication>

extern "C" void gdk_init(int *argc, char ***argv);

int main(int argc, char *argv[])
{
    gdk_init(0, NULL);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
