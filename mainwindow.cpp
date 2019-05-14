#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("SY1527");

    ui->statusBar->showMessage("Connecting to 172.22.4.1 ...", 1000);
    timer.singleShot(1000, &hvp, &HVSystem::Login );

    ui->sbVStart->setValue(1800);
    ui->sbVStop->setValue(2400);
    ui->sbVStep->setValue(10);
    ui->pbStopHVScan->setEnabled(false);

    initGUI();
    createConnections();
}

MainWindow::~MainWindow()
{
    if (tmrInfoChannel.isActive())
        tmrInfoChannel.stop();
    delete ui;
}

void MainWindow::initGUI()
{
    ui->sbVoltage_1->setEnabled( false );
    ui->sbVoltage_1->setKeyboardTracking(false);
    ui->lblVoltage_1->setEnabled( false );
    ui->lblCurrent_1->setEnabled( false );
    lsWChannels[0].state = ui->chbChannel_1;
    lsWChannels[0].svolt = ui->sbVoltage_1;
    lsWChannels[0].mvolt = ui->lblVoltage_1;
    lsWChannels[0].curr  = ui->lblCurrent_1;

    ui->sbVoltage_2->setEnabled( false );
    ui->sbVoltage_2->setKeyboardTracking(false);
    ui->lblVoltage_2->setEnabled( false );
    ui->lblCurrent_2->setEnabled( false );
    lsWChannels[1].state = ui->chbChannel_2;
    lsWChannels[1].svolt = ui->sbVoltage_2;
    lsWChannels[1].mvolt = ui->lblVoltage_2;
    lsWChannels[1].curr  = ui->lblCurrent_2;

    ui->sbVoltage_3->setEnabled( false );
    ui->sbVoltage_3->setKeyboardTracking(false);
    ui->lblVoltage_3->setEnabled( false );
    ui->lblCurrent_3->setEnabled( false );
    lsWChannels[2].state = ui->chbChannel_3;
    lsWChannels[2].svolt = ui->sbVoltage_3;
    lsWChannels[2].mvolt = ui->lblVoltage_3;
    lsWChannels[2].curr  = ui->lblCurrent_3;

    ui->sbVoltage_4->setEnabled( false );
    ui->sbVoltage_4->setKeyboardTracking(false);
    ui->lblVoltage_4->setEnabled( false );
    ui->lblCurrent_4->setEnabled( false );
    lsWChannels[3].state = ui->chbChannel_4;
    lsWChannels[3].svolt = ui->sbVoltage_4;
    lsWChannels[3].mvolt = ui->lblVoltage_4;
    lsWChannels[3].curr  = ui->lblCurrent_4;

    ui->sbVoltage_5->setEnabled( false );
    ui->sbVoltage_5->setKeyboardTracking(false);
    ui->lblVoltage_5->setEnabled( false );
    ui->lblCurrent_5->setEnabled( false );
    lsWChannels[4].state = ui->chbChannel_5;
    lsWChannels[4].svolt = ui->sbVoltage_5;
    lsWChannels[4].mvolt = ui->lblVoltage_5;
    lsWChannels[4].curr  = ui->lblCurrent_5;

    ui->sbVoltage_6->setEnabled( false );
    ui->sbVoltage_6->setKeyboardTracking(false);
    ui->lblVoltage_6->setEnabled( false );
    ui->lblCurrent_6->setEnabled( false );
    lsWChannels[5].state = ui->chbChannel_6;
    lsWChannels[5].svolt = ui->sbVoltage_6;
    lsWChannels[5].mvolt = ui->lblVoltage_6;
    lsWChannels[5].curr  = ui->lblCurrent_6;

    ui->sbVoltage_7->setEnabled( false );
    ui->sbVoltage_7->setKeyboardTracking(false);
    ui->lblVoltage_7->setEnabled( false );
    ui->lblCurrent_7->setEnabled( false );
    lsWChannels[6].state = ui->chbChannel_7;
    lsWChannels[6].svolt = ui->sbVoltage_7;
    lsWChannels[6].mvolt = ui->lblVoltage_7;
    lsWChannels[6].curr  = ui->lblCurrent_7;

    ui->sbVoltage_8->setEnabled( false );
    ui->sbVoltage_8->setKeyboardTracking(false);
    ui->lblVoltage_8->setEnabled( false );
    ui->lblCurrent_8->setEnabled( false );
    lsWChannels[7].state = ui->chbChannel_8;
    lsWChannels[7].svolt  = ui->sbVoltage_8;
    lsWChannels[7].mvolt = ui->lblVoltage_8;
    lsWChannels[7].curr  = ui->lblCurrent_8;

    ui->sbVoltage_9->setEnabled( false );
    ui->sbVoltage_9->setKeyboardTracking(false);
    ui->lblVoltage_9->setEnabled( false );
    ui->lblCurrent_9->setEnabled( false );
    lsWChannels[8].state = ui->chbChannel_9;
    lsWChannels[8].svolt  = ui->sbVoltage_9;
    lsWChannels[8].mvolt = ui->lblVoltage_9;
    lsWChannels[8].curr  = ui->lblCurrent_9;

    ui->sbVoltage_10->setEnabled( false );
    ui->sbVoltage_10->setKeyboardTracking(false);
    ui->lblVoltage_10->setEnabled( false );
    ui->lblCurrent_10->setEnabled( false );
    lsWChannels[9].state = ui->chbChannel_10;
    lsWChannels[9].svolt  = ui->sbVoltage_10;
    lsWChannels[9].mvolt = ui->lblVoltage_10;
    lsWChannels[9].curr  = ui->lblCurrent_10;

    ui->sbVoltage_11->setEnabled( false );
    ui->sbVoltage_11->setKeyboardTracking(false);
    ui->lblVoltage_11->setEnabled( false );
    ui->lblCurrent_11->setEnabled( false );
    lsWChannels[10].state = ui->chbChannel_11;
    lsWChannels[10].svolt  = ui->sbVoltage_11;
    lsWChannels[10].mvolt = ui->lblVoltage_11;
    lsWChannels[10].curr  = ui->lblCurrent_11;

    ui->sbVoltage_12->setEnabled( false );
    ui->sbVoltage_12->setKeyboardTracking(false);
    ui->lblVoltage_12->setEnabled( false );
    ui->lblCurrent_12->setEnabled( false );
    lsWChannels[11].state = ui->chbChannel_12;
    lsWChannels[11].svolt  = ui->sbVoltage_12;
    lsWChannels[11].mvolt = ui->lblVoltage_12;
    lsWChannels[11].curr  = ui->lblCurrent_12;
}

void MainWindow::createConnections()
{
    qDebug() << "MainWindow::CreateConnection()";

    connect(ui->actionCrateMap,      &QAction::triggered,   &hvp, &HVSystem::getCrateMap);
    connect(ui->pbStartHVScan,       &QPushButton::clicked, this, &MainWindow::slStartHVScan);
    connect(ui->pbStopHVScan,        &QPushButton::clicked, this, &MainWindow::slStopHVScan);
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
    //connect(ui->sbVoltage_8, &QSpinBox::editingFinished, [this](){qDebug() << "value volt_1 =" << ui->sbVoltage_8->value();});
/*
*/
    connect(ui->sbVoltage_1,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_2,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_3,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_4,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_5,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_6,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_7,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_8,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_9,  QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_10, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_11, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_12, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::slChangeVoltChannel);
/*
    connect(ui->sbVoltage_1,  &QSpinBox::editingFinished, this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_2,  &QSpinBox::editingFinished, this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_3,  &QSpinBox::editingFinished, this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_4,  &QSpinBox::editingFinished, this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_5,  &QSpinBox::editingFinished, this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_6,  &QSpinBox::editingFinished, this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_7,  &QSpinBox::editingFinished, this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_8,  &QSpinBox::editingFinished, this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_9,  &QSpinBox::editingFinished, this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_10, &QSpinBox::editingFinished, this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_11, &QSpinBox::editingFinished, this, &MainWindow::slChangeVoltChannel);
    connect(ui->sbVoltage_12, &QSpinBox::editingFinished, this, &MainWindow::slChangeVoltChannel);
*/

    connect(&hvp, &HVSystem::sgnLogged, this, &MainWindow::slConnectHVP);
    //connect(&hvp, &HVSystem::sendMessage, ui->statusBar, &QStatusBar::showMessage);
    connect(&hvp, &HVSystem::sendMessage, [this](QString str){ui->statusBar->showMessage(str, 5000);});

}

void MainWindow::slChangeStateChannel()
{
    qDebug() << "MainWindow::slChangeStateChannel ...";

    QCheckBox* p_chan = qobject_cast<QCheckBox*>( sender() );
    for (uint8_t i {0}; i < nmChannels; ++i){
        if ( lsWChannels[i].state == p_chan){
            bool f_state = p_chan->isChecked();
            hvp.setPowerChannel(i, f_state);

            lsWChannels[i].svolt->setEnabled( f_state );
            lsWChannels[i].mvolt->setEnabled( f_state );
            lsWChannels[i].curr->setEnabled( f_state );
        }
    }
}

void MainWindow::slChangeVoltChannel(int value)
//void MainWindow::slChangeVoltChannel()
{
    qDebug() << "MainWindow::slChangeVoltChannel ...";

    QSpinBox* p_wdgVolt = qobject_cast<QSpinBox*>( sender() );
    for (uint8_t i{0}; i < nmChannels; ++i) {
        if ( lsWChannels[i].svolt == p_wdgVolt){
            hvp.setVoltageChannel(i, p_wdgVolt->value());
        }
    }
}

void MainWindow::slStartHVScan()
{
    auto volStart = static_cast<uint16_t> (ui->sbVStart->value());
    auto volStop  = static_cast<uint16_t> (ui->sbVStop->value());
    auto volStep  = static_cast<uint8_t> (ui->sbVStep->value());

    ui->pbStartHVScan->setEnabled(false);
    ui->pbStopHVScan->setEnabled(true);

    hvs.setHVPower(&hvp);
    hvs.setVoltageRange(volStart, volStop);
    hvs.setVoltageStep (volStep);
    hvs.setTime(ui->sbHVSTime->value());
    hvs.start();
}

void MainWindow::slStopHVScan()
{
    ui->pbStartHVScan->setEnabled(true);
    ui->pbStopHVScan->setEnabled(false);

    hvs.stopHVScan();
}

void MainWindow::slSetNamesChannels()
{
    qDebug() << "MainWindow::slSetNamesChannels() ...";

    for (size_t i {0}; i < nmChannels; ++i) {
        lsWChannels[i].state->setText( hvp.arrChan[i].name );
    }
}

void MainWindow::slConnectHVP(bool state)
{
    qDebug() << "MainWindow::slConnectHVP() ...";

    if (state){
        slSetNamesChannels();        
        for (uint8_t i {0}; i < nmChannels; ++i){
            if ( hvp.arrChan[i].Pw ){
                lsWChannels[i].state->setChecked(true);
                lsWChannels[i].svolt->setEnabled(true);
                lsWChannels[i].mvolt->setEnabled(true);
                lsWChannels[i].curr->setEnabled (true);
            }
        }
        tmrInfoChannel.start(1000);
    }
    else {
        // show window with info about error
//        ui->statusBar->showMessage();

    }
}

void MainWindow::slTest()
{
    qDebug() << "MainWindow::slTest ... ";

}

void MainWindow::slGetInfoChannels()
{
    //qDebug() << "MainWindow::slGetInfoChannels() ...";


    hvp.getChannelParameters("VMon");
    hvp.getChannelParameters("IMon");
    hvp.getChannelParameters("Pw");

    //qDebug() << "\nInfo channels:";
    for (size_t i {0}; i < nmChannels; ++i) {
        if (0){
            qDebug() << "VMon =" << hvp.arrChan[i].VMon
                     << "IMon =" << hvp.arrChan[i].IMon
                     << "State =" << hvp.arrChan[i].Pw;
        }

        lsWChannels[i].state->setChecked( hvp.arrChan[i].Pw );
        lsWChannels[i].mvolt->setText( QString::number(hvp.arrChan[i].VMon) + " V");
        lsWChannels[i].curr->setText( QString::number(hvp.arrChan[i].IMon) );
    }
}
