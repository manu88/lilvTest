#include "mainwindow.h"
#include <QDebug>
#include <QPushButton>
#include <QToolButton>
#include <QWidget>
#include <QWindow>
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->refreshButton, &QToolButton::clicked, this, &MainWindow::updateListClicked);
    setWindowTitle("LV2 plugin explorer");
    LV2::Plugin::manager().refreshPlugins();
    populatePluginList();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateListClicked()
{
    LV2::Plugin::manager().refreshPlugins();
    populatePluginList();
}

void MainWindow::populatePluginList()
{
    int numCols = 3;
    ui->treeWidget->setColumnCount(numCols);
    QStringList columnNames;
    columnNames << "name" << "uri" << "project";
    ui->treeWidget->setHeaderLabels(QStringList(columnNames));

    QList<QTreeWidgetItem *> items;

    const auto plugins = LV2::Plugin::manager().getPlugins();
    for (auto const &p : plugins) {
        QTreeWidgetItem *pluginWidget = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),
                                                            QStringList(p.name));

        pluginWidget->setText(1, p.uri);
        pluginWidget->setText(2, p.project);

        QTreeWidgetItem *bundleWidget = new QTreeWidgetItem();
        bundleWidget->setText(0, "bundle");
        bundleWidget->setText(1, p.bundleUri);
        pluginWidget->addChild(bundleWidget);

        QTreeWidgetItem *portsWidget = new QTreeWidgetItem();
        portsWidget->setText(0, "ports");
        pluginWidget->addChild(portsWidget);
        for (int i = 0; i < p.ports.size(); i++) {
            const QString desc = QString::number(i) + " '" + p.ports[i].name + "' "
                                 + (LV2::Plugin::Description::Port::AUDIO ? "audio" : "control")
                                 + " "
                                 + (LV2::Plugin::Description::Port::INPUT ? "input" : "output")
                                 + " " + (p.ports[i].optional ? "(optional)" : "");
            QTreeWidgetItem *portWidget = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),
                                                              QStringList(desc));

            if (!p.ports[i].scalePoints.empty()) {
                QTreeWidgetItem *scalePointsWidget = new QTreeWidgetItem();
                scalePointsWidget->setText(0, "scale points");
                for (auto const &p : p.ports[i].scalePoints) {
                    QString scalePointDesc = p.label + ": " + QString::number(p.value);
                    QTreeWidgetItem *scalePointWidget
                        = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),
                                              QStringList(scalePointDesc));
                    scalePointsWidget->addChild(scalePointWidget);
                }
                portWidget->addChild(scalePointsWidget);
            }
            portsWidget->addChild(portWidget);
        }

        QTreeWidgetItem *featsWidget = new QTreeWidgetItem();
        featsWidget->setText(0, "features");
        pluginWidget->addChild(featsWidget);

        for (auto const &feat : p.features) {
            QTreeWidgetItem *featWidget = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr),
                                                              QStringList((feat.optional
                                                                               ? "optional"
                                                                               : "required")));
            featWidget->setText(1, feat.uri);
            featsWidget->addChild(featWidget);
        }

        if (!p.uis.empty()) {
            QTreeWidgetItem *uisWidget = new QTreeWidgetItem();
            uisWidget->setText(0, "uis");
            pluginWidget->addChild(uisWidget);
            for (auto const &ui : p.uis) {
                QTreeWidgetItem *uiWidget = new QTreeWidgetItem();
                uiWidget->setText(0, ui.nativeUIType);
                uiWidget->setText(1, ui.uri);
                uisWidget->addChild(uiWidget);
            }
        }

        items.append(pluginWidget);
    }
    ui->treeWidget->clear();
    ui->treeWidget->insertTopLevelItems(0, items);
    for (int i = 0; i < numCols; i++) {
        ui->treeWidget->resizeColumnToContents(i);
    }
}
