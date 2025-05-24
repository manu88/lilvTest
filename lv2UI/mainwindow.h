#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "plugins.h"
#include "uihost.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void updateListClicked();

private:
    void populatePluginList();
    Ui::MainWindow *ui;

    UIHost _uiHost;
    LV2::Plugin _plugins;

};
#endif // MAINWINDOW_H
