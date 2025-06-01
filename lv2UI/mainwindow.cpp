#include "mainwindow.h"
#include <QDebug>
#include <QPushButton>
#include <QToolButton>
#include <QWidget>
#include <QWindow>
#include "./ui_mainwindow.h"
#include "pluginManager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    setWindowTitle("LV2 plugin explorer");
    ui->setupUi(this);
    ui->deleteUIButton->setDisabled(true);
    connect(ui->refreshButton, &QToolButton::clicked, this, &MainWindow::updateListClicked);
    connect(ui->createUIButton, &QToolButton::clicked, this, &MainWindow::createUIClicked);

    connect(ui->uiInstanceListWidget,
            &QListWidget::itemClicked,
            this,
            &MainWindow::uiInstanceListItemChanged);

    connect(ui->deleteUIButton, &QPushButton::clicked, this, &MainWindow::deleteUIInstanceClicked);
    LV2::Plugin::manager().refreshPlugins();
    populatePluginList();

    connect(&_pluginUIManager,
            &LV2::UI::Manager::instancesChanged,
            this,
            &MainWindow::updateUIInstanceList);
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

void MainWindow::createUIClicked(){
    auto pluginIndex = ui->treeWidget->currentIndex();
    if (pluginIndex.row() <0){
        return;
    }
    const auto pluginDesc =  LV2::Plugin::manager().getPlugins().at(pluginIndex.row());
    qDebug("plugin selected index %i : %s", pluginIndex.row(), pluginDesc.uri.toStdString().c_str());

    if (_pluginUIManager.createInstanceFor(pluginDesc)) {
        updateUIInstanceList();
        int index = _pluginUIManager.getInstances().size() - 1;
        ui->uiInstanceListWidget->setCurrentRow(index);
        populateUIInstanceDescriptionFrameFrom(_pluginUIManager.getInstances().at(index));
    }
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
                QString txt = ui.uiType + (ui.isNative ? " (native)" : "");
                uiWidget->setText(0, txt);
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

void MainWindow::updateUIInstanceList()
{
    ui->uiInstanceListWidget->clear();
    for (const auto &instance : _pluginUIManager.getInstances()) {
        new QListWidgetItem(instance.desc.name, ui->uiInstanceListWidget);
    }
}

void MainWindow::uiInstanceListItemChanged(QListWidgetItem *item)
{
    int index = ui->uiInstanceListWidget->currentRow();
    qDebug("item changed to %i", index);
    auto instance = _pluginUIManager.getInstances().at(index);
    populateUIInstanceDescriptionFrameFrom(instance);
}

void MainWindow::populateUIInstanceDescriptionFrameFrom(const LV2::UI::Instance &instance)
{
    ui->labelName->setText(instance.desc.name);
    ui->labelUri->setText(instance.desc.uri);
    ui->labelUIStatus->setText("some status");
    ui->labelUUID->setText(instance.uuid);
    ui->deleteUIButton->setDisabled(false);
}

void MainWindow::deleteUIInstanceClicked()
{
    auto instance = _pluginUIManager.getInstances().at(ui->uiInstanceListWidget->currentRow());
    qDebug("delete instance %s", instance.desc.name.toStdString().c_str());
    _pluginUIManager.deleteInstance(instance.uuid);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug("quit app");
    _pluginUIManager.cleanup();
    QMainWindow::closeEvent(event);
}
