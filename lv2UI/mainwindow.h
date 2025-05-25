#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pluginManager.h"
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

    LV2::Plugin::UIHost _uiHost;

};
#endif // MAINWINDOW_H
