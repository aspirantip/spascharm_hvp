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

    initGUI();
    createConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initGUI()
{
    lsWChannels[0].state = ui->chbChannel_1;
    lsWChannels[0].volt  = ui->sbVoltage_1;     ui->sbVoltage_1->setEnabled( false );
    lsWChannels[0].curr  = ui->lblCurrent_1;    ui->lblCurrent_1->setEnabled( false );
    lsWChannels[1].state = ui->chbChannel_2;
    lsWChannels[1].volt  = ui->sbVoltage_2;     ui->sbVoltage_2->setEnabled( false );
    lsWChannels[1].curr  = ui->lblCurrent_2;    ui->lblCurrent_2->setEnabled( false );
    lsWChannels[2].state = ui->chbChannel_3;
    lsWChannels[2].volt  = ui->sbVoltage_3;     ui->sbVoltage_3->setEnabled( false );
    lsWChannels[2].curr  = ui->lblCurrent_3;    ui->lblCurrent_3->setEnabled( false );
    lsWChannels[3].state = ui->chbChannel_4;
    lsWChannels[3].volt  = ui->sbVoltage_4;     ui->sbVoltage_4->setEnabled( false );
    lsWChannels[3].curr  = ui->lblCurrent_4;    ui->lblCurrent_4->setEnabled( false );
    lsWChannels[4].state = ui->chbChannel_5;
    lsWChannels[4].volt  = ui->sbVoltage_5;     ui->sbVoltage_5->setEnabled( false );
    lsWChannels[4].curr  = ui->lblCurrent_5;    ui->lblCurrent_5->setEnabled( false );
    lsWChannels[5].state = ui->chbChannel_6;
    lsWChannels[5].volt  = ui->sbVoltage_6;     ui->sbVoltage_6->setEnabled( false );
    lsWChannels[5].curr  = ui->lblCurrent_6;    ui->lblCurrent_6->setEnabled( false );
    lsWChannels[6].state = ui->chbChannel_7;
    lsWChannels[6].volt  = ui->sbVoltage_7;     ui->sbVoltage_7->setEnabled( false );
    lsWChannels[6].curr  = ui->lblCurrent_7;    ui->lblCurrent_7->setEnabled( false );
    lsWChannels[7].state = ui->chbChannel_8;
    lsWChannels[7].volt  = ui->sbVoltage_8;     ui->sbVoltage_8->setEnabled( false );
    lsWChannels[7].curr  = ui->lblCurrent_8;    ui->lblCurrent_8->setEnabled( false );
    lsWChannels[8].state = ui->chbChannel_9;
    lsWChannels[8].volt  = ui->sbVoltage_9;     ui->sbVoltage_9->setEnabled( false );
    lsWChannels[8].curr  = ui->lblCurrent_9;    ui->lblCurrent_9->setEnabled( false );
    lsWChannels[9].state = ui->chbChannel_10;
    lsWChannels[9].volt  = ui->sbVoltage_10;    ui->sbVoltage_10->setEnabled( false );
    lsWChannels[9].curr  = ui->lblCurrent_10;   ui->lblCurrent_10->setEnabled( false );
    lsWChannels[10].state = ui->chbChannel_11;
    lsWChannels[10].volt  = ui->sbVoltage_11;   ui->sbVoltage_11->setEnabled( false );
    lsWChannels[10].curr  = ui->lblCurrent_11;  ui->lblCurrent_11->setEnabled( false );
    lsWChannels[11].state = ui->chbChannel_12;
    lsWChannels[11].volt  = ui->sbVoltage_12;   ui->sbVoltage_12->setEnabled( false );
    lsWChannels[11].curr  = ui->lblCurrent_12;  ui->lblCurrent_12->setEnabled( false );


}

void MainWindow::createConnections()
{
    qDebug() << "MainWindow::CreateConnection()";

    connect(ui->pbCrateMap,          &QPushButton::clicked, &hvs, &HVSystem::getCrateMap);
    connect(ui->pbGetChannelName,    &QPushButton::clicked, &hvs, &HVSystem::getChannelName);
    connect(ui->pbSetVoltage,        &QPushButton::clicked, this, &MainWindow::slTest);
    connect(ui->pbStartHVScan,       &QPushButton::clicked, this, &MainWindow::slStartHVScan);

    connect(&tmrInfoChannel, &QTimer::timeout, this, &MainWindow::slGetInfoChannels);


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


    //connect(ui->sbVoltage_1, QOverload<int>::of(&QSpinBox::valueChanged), [](int val){qDebug() << "value =" << val;} );
    connect(ui->sbVoltage_1, &QSpinBox::editingFinished, [this](){qDebug() << "value volt_1 =" << ui->sbVoltage_1->value();});

    connect(&hvs, &HVSystem::sgnLogged, this, &MainWindow::slConnectHVP);
    //connect(&hvs, &HVSystem::sendMessage, ui->statusBar, &QStatusBar::showMessage);
    connect(&hvs, &HVSystem::sendMessage, [this](QString str){ui->statusBar->showMessage(str, 5000);});

}

void MainWindow::slChangeStateChannel()
{
    qDebug() << "MainWindow::slChangeStateChannel ...";

    QCheckBox* p_chan = qobject_cast<QCheckBox*>( sender() );

    for (uint8_t i {0}; i < nmChannels; ++i){
        if ( lsWChannels[i].state == p_chan){
            bool f_state = p_chan->isChecked();
            hvs.setPowerChannel(i, f_state);

            lsWChannels[i].volt->setEnabled( f_state );
            lsWChannels[i].curr->setEnabled( f_state );
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


    for (auto crVolt {volStart}; crVolt <= volStop; crVolt += volStep)
    {
        // [1] set voltage
        qDebug() << "\n   Set voltage " << crVolt;
        hvs.setVoltageSystem( crVolt );


        QDir dir;
        bool f_state = dir.mkdir( QString::number(crVolt) );
        qDebug() << "dir =" << dir.path();
        qDebug() << "dir =" << dir.absolutePath();


        QProcess procSetNameFile;
        procSetNameFile.start( "sh", QStringList() << "-c" << QString("echo 'volt = %1 ' > ./%1/file_name_.text").arg(crVolt) );
        procSetNameFile.waitForFinished();
        procSetNameFile.close();


        // [2] delay or monitoring current
        qDebug() << "   Delay ... ";
        QThread::sleep( 3 );


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

    for (size_t i {0}; i < nmChannels; ++i) {
        lsWChannels[i].state->setText( hvs.arrChan[i].name );
    }
}

void MainWindow::slConnectHVP(bool state)
{
    qDebug() << "MainWindow::slConnectHVP() ...";

    if (state){
        slSetNamesChannels();
        tmrInfoChannel.start(1000);
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
    hvs.getChannelParameters("IMon");
    hvs.getChannelParameters("Pw");

    qDebug() << "\nInfo channels:";
    for (size_t i {0}; i < nmChannels; ++i) {
        qDebug() << "VMon =" << hvs.arrChan[i].VMon
                 << "IMon =" << hvs.arrChan[i].IMon
                 << "State =" << hvs.arrChan[i].Pw;

        lsWChannels[i].state->setChecked( hvs.arrChan[i].Pw );
        lsWChannels[i].volt->setValue( hvs.arrChan[i].VMon );
        lsWChannels[i].curr->setText( QString::number(hvs.arrChan[i].IMon) );

    }
}
