#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const auto plugins = LV2::Plugin::manager().enumeratePlugins();
    for(auto const &p: plugins){
        qDebug("plugin '%s' '%s'", p.name.toStdString().c_str(), p.uri.toStdString().c_str());
        qDebug("\tports:");
        for(int i=0;i <p.ports.size(); i++){
            qDebug("\t\tport %i '%s' %s %s %s",
                   i,
                   p.ports[i].name.toStdString().c_str(),
                   p.ports[i].type == LV2::Plugin::Description::Port::AUDIO? "audio":"control",
                   p.ports[i].flow == LV2::Plugin::Description::Port::INPUT? "input":"output",
                   p.ports[i].optional ? "(optional)":"");
        }

        qDebug("\tfeatures:");
        for(auto const &feat: p.features){
            qDebug("\t\t%s %s", feat.uri.toStdString().c_str(), feat.optional? "optional":"required");
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
