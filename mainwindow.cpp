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



    vCheckBoxChannels.resize(nmChannels);
    vCheckBoxChannels[0] = ui->chbChannel_1;
    vCheckBoxChannels[1] = ui->chbChannel_2;
    vCheckBoxChannels[2] = ui->chbChannel_3;
    vCheckBoxChannels[3] = ui->chbChannel_4;
    vCheckBoxChannels[4] = ui->chbChannel_5;
    vCheckBoxChannels[5] = ui->chbChannel_6;
    vCheckBoxChannels[6] = ui->chbChannel_7;
    vCheckBoxChannels[7] = ui->chbChannel_8;
    vCheckBoxChannels[8] = ui->chbChannel_9;
    vCheckBoxChannels[9] = ui->chbChannel_10;
    vCheckBoxChannels[10] = ui->chbChannel_11;
    vCheckBoxChannels[11] = ui->chbChannel_12;


    //ui->pbGetChannelVoltage->setEnabled( false );

    createConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createConnections()
{
    qDebug() << "MainWindow::CreateConnection()";

    connect(ui->pbCrateMap,          &QPushButton::clicked, &hvs, &HVSystem::getCrateMap);
    connect(ui->pbGetChannelName,    &QPushButton::clicked, &hvs, &HVSystem::getChannelName);
//    connect(ui->pbGetChannelVoltage, &QPushButton::clicked, &hvs, &HVSystem::getChannelParameters);
//    connect(ui->pbSetVoltage,        &QPushButton::clicked, &hvs, &HVSystem::setChannelParameters);
    connect(ui->pbSetVoltage,        &QPushButton::clicked, this, &MainWindow::slTest);
    connect(ui->pbStartHVScan,       &QPushButton::clicked, this, &MainWindow::slStartHVScan);

    //connect(&tmrInfoChannel, &QTimer::timeout, this, &MainWindow::slGetInfoChannels);
    connect(ui->pbGetChannelVoltage, &QPushButton::clicked, this, &MainWindow::slGetInfoChannels);


    connect(ui->chbChannel_1, &QCheckBox::clicked, this, &MainWindow::slChangeStateChannel);
    connect(ui->chbChannel_2, &QCheckBox::clicked, this, &MainWindow::slChangeStateChannel);
    connect(ui->chbChannel_3, &QCheckBox::clicked, this, &MainWindow::slChangeStateChannel);
    connect(ui->chbChannel_4, &QCheckBox::clicked, this, &MainWindow::slChangeStateChannel);
    connect(ui->chbChannel_5, &QCheckBox::clicked, this, &MainWindow::slChangeStateChannel);
    connect(ui->chbChannel_6, &QCheckBox::clicked, this, &MainWindow::slChangeStateChannel);
    connect(ui->chbChannel_7, &QCheckBox::clicked, this, &MainWindow::slChangeStateChannel);
    connect(ui->chbChannel_8, &QCheckBox::clicked, this, &MainWindow::slChangeStateChannel);
    connect(ui->chbChannel_9, &QCheckBox::clicked, this, &MainWindow::slChangeStateChannel);
    connect(ui->chbChannel_10, &QCheckBox::clicked, this, &MainWindow::slChangeStateChannel);
    connect(ui->chbChannel_11, &QCheckBox::clicked, this, &MainWindow::slChangeStateChannel);
    connect(ui->chbChannel_12, &QCheckBox::clicked, this, &MainWindow::slChangeStateChannel);


    connect(ui->sbVoltage_1, QOverload<int>::of(&QSpinBox::valueChanged), [](int val){qDebug() << "value =" << val;} );


    connect(&hvs, &HVSystem::sgnLogged, this, &MainWindow::slConnectHVP);

}

void MainWindow::slChangeStateChannel()
{
    qDebug() << "MainWindow::slChangeStateChannel ...";

    QCheckBox* chan = static_cast<QCheckBox*>( sender() );
    //qDebug() << "state:" << chan->objectName();
    //qDebug() << "state:" << chan->isChecked();

    for (uint8_t i {0}; i < nmChannels; ++i){
        //qDebug() << "#" << i << " | state:" << vCheckBoxChannels[i]->isChecked();
        //qDebug() << vCheckBoxChannels[i] << " | " << chan;
        if (vCheckBoxChannels[i] == chan){
            //qDebug() << "Clicked" << i << "check box";
            hvs.setPowerChannel(i, chan->isChecked());
        }
    }


}

void MainWindow::slChangeVoltChannel(int value)
{
    Q_UNUSED(value);

    QSpinBox* wdg = static_cast<QSpinBox*>( sender() );
    qDebug() << "value =" << wdg->value();

}

void MainWindow::slStartHVScan()
{
    qDebug() << "\n===============================";
    qDebug() << "Start HVScan ...";

    uint32_t volStart = static_cast<uint32_t> (ui->sbVStart->value());
    uint32_t volStop  = static_cast<uint32_t> (ui->sbVStop->value());
    uint32_t volStep  = static_cast<uint32_t> (ui->sbVStep->value());


    hvs.printActiveChannels();


    for (auto crVolt {volStart}; crVolt <= volStop; crVolt += volStep)
    {
        // [1] set voltage
        qDebug() << "\n   Set voltage " << crVolt;
        //hvs.setVoltage();
        hvs.setVoltageSystem( crVolt );


        // [2] delay or monitoring current
        qDebug() << "   Delay ... ";
        QThread::sleep( 15 );


        // [3] data acquisition
        //      [3.1] data processing
        //      [3.2] data visualization
        qDebug() << "   Run DAQ ...";
    }

    qDebug() << "Stop HVScan ...";
    qDebug() << "===============================";
}

void MainWindow::slSetNamesChannels()
{
    qDebug() << "MainWindow::slSetNamesChannels() ...";

    for (auto i {0}; i < nmChannels; ++i) {
        vCheckBoxChannels[i]->setText( hvs.arrChan[i].name );
    }
}

void MainWindow::slConnectHVP(bool state)
{
    qDebug() << "MainWindow::slConnectHVP() ...";

    if (state){
        slSetNamesChannels();
        tmrInfoChannel.start(3000);
    }
    else {
        // show window with info about error

    }
}

void MainWindow::slTest()
{

    hvs.setVoltageSystem( 200.0 );
}

void MainWindow::slGetInfoChannels()
{
    qDebug() << "MainWindow::slGetInfoChannels() ...";


    hvs.getChannelParameters("VMon");
    //hvs.getChannelParameters("IMon");
    //hvs.getChannelParameters("Pw");

    //hvs.arrChan[ind]->VMon;
    qDebug() << "Info channels:";
    for (auto i {0}; i < nmChannels; ++i) {
        qDebug() << "VMon =" << hvs.arrChan[i].VMon
                 << "IMon =" << hvs.arrChan[i].IMon
                 << "State =" << hvs.arrChan[i].Pw;



    }

}
