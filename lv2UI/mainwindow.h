#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pluginManager.h"
#include "uimanager.h"

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

private Q_SLOTS:
    void updateListClicked();
    void createUIClicked();

private:
    void populatePluginList();
    Ui::MainWindow *ui;

    LV2::UI::Manager _pluginUIManager;
};
#endif // MAINWINDOW_H
