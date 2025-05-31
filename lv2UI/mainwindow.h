#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidget>
#include <QMainWindow>
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
    void uiInstanceListItemChanged(QListWidgetItem *item);
    void deleteUIInstanceClicked();

private:
    void updateUIInstanceList();
    void populatePluginList();
    void populateUIInstanceDescriptionFrameFrom(const LV2::UI::Instance &instance);
    Ui::MainWindow *ui;

    LV2::UI::Manager _pluginUIManager;
};
#endif // MAINWINDOW_H
