#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    populatePluginList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::populatePluginList(){
    int numCols = 2;
    ui->treeWidget->setColumnCount(numCols);
    QStringList columnNames;
    columnNames << "name" << "uri";
    ui->treeWidget->setHeaderLabels(QStringList(columnNames));

    QList<QTreeWidgetItem *> items;

    const auto plugins = LV2::Plugin::manager().enumeratePlugins();
    for(auto const &p: plugins){
        QTreeWidgetItem* pluginWidget = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList(p.name));

        pluginWidget->setText(1, p.uri);


        QTreeWidgetItem* portsWidget = new QTreeWidgetItem();
        portsWidget->setText(0, "ports");
        pluginWidget->addChild(portsWidget);
        for(int i=0;i <p.ports.size(); i++){

            const QString desc = QString::number(i) + " '" + p.ports[i].name + "' " + (LV2::Plugin::Description::Port::AUDIO? "audio":"control")  + " " + (LV2::Plugin::Description::Port::INPUT? "input":"output") + " " + (p.ports[i].optional ? "(optional)":"");
            QTreeWidgetItem* portWidget = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList(desc));
            portsWidget->addChild(portWidget);
        }

        QTreeWidgetItem* featsWidget = new QTreeWidgetItem();
        featsWidget->setText(0, "features");
        pluginWidget->addChild(featsWidget);

        for(auto const &feat: p.features){
            QTreeWidgetItem* featWidget = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList((feat.optional? "optional":"required")));
            featWidget->setText(1, feat.uri);
            featsWidget->addChild(featWidget);
        }

        items.append(pluginWidget);
    }

    ui->treeWidget->insertTopLevelItems(0, items);
    for(int i = 0; i < numCols ; i++)
    {
        ui->treeWidget->resizeColumnToContents(i);
    }
}
