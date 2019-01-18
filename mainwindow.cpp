#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("SY1527");

    ui->statusBar->showMessage("Connecting to 172.22.4.1 ...", 1000);
    timer.singleShot(1000, &hvs, &HVSystem::Login );

    CreateConnection();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::CreateConnection()
{
    qDebug() << "MainWindow::CreateConnection()";
    connect(ui->pbCrateMap, &QPushButton::clicked, &hvs, &HVSystem::getCrateMap);
    connect(ui->pbGetChannelName, &QPushButton::clicked, &hvs, &HVSystem::getChannelName);
    connect(ui->pbGetChannelVoltage, &QPushButton::clicked, &hvs, &HVSystem::getChannelParameters);
    connect(ui->pbSetVoltage,   &QPushButton::clicked, &hvs, &HVSystem::setChannelParameters);


}
