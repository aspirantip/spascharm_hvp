#include "mainwindow.h"
#include "ui_mainwindow.h"


bool verify_host(ssh_session session)
{

    unsigned char *hash = nullptr;
    char *hexa;
    char buf[10];

    int state = ssh_is_server_known(session);
    int hlen  = ssh_get_pubkey_hash(session, &hash);
    if (hlen < 0)
        return false;

    switch (state)
    {
    case SSH_SERVER_KNOWN_OK:
        std::cout << "Server known (OK): " << ssh_get_hexa(hash, hlen) << std::endl;
        break; /* ok */
    case SSH_SERVER_KNOWN_CHANGED:
        fprintf(stderr, "Host key for server changed: it is now:\n");
        ssh_print_hexa("Public key hash", hash, hlen);
        fprintf(stderr, "For security reasons, connection will be stopped\n");
        free(hash);
        return false;
    case SSH_SERVER_FOUND_OTHER:
        fprintf(stderr, "The host key for this server was not found but an other"
                        "type of key exists.\n");
        fprintf(stderr, "An attacker might change the default server key to"
                        "confuse your client into thinking the key does not exist\n");
        free(hash);
        return false;
    case SSH_SERVER_FILE_NOT_FOUND:
        fprintf(stderr, "Could not find known host file.\n");
        fprintf(stderr, "If you accept the host key here, the file will be"
                        "automatically created.\n");
        /* fallback to SSH_SERVER_NOT_KNOWN behavior */
    case SSH_SERVER_NOT_KNOWN:
        hexa = ssh_get_hexa(hash, hlen);
        fprintf(stderr,"The server is unknown. Do you trust the host key [yes/no]?\n");
        fprintf(stderr, "Public key hash: %s\n", hexa);
        free(hexa);
        if (fgets(buf, sizeof(buf), stdin) == nullptr){
            free(hash);
            return false;
        }
        if (strncasecmp(buf, "yes", 3) != 0)        {
            free(hash);
            return false;
        }
        if (ssh_write_knownhost(session) < 0)        {
            fprintf(stderr, "Error %s\n", strerror(errno));
            free(hash);
            return false;
        }
        break;
    case SSH_SERVER_ERROR:
        fprintf(stderr, "Error %s", ssh_get_error(session));
        free(hash);
        return false;
    }

    free(hash);
    return true;
}


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

void MainWindow::makeDirectory(QString name_dir)
{
    //RUN=run263_2000_st3_parallel_flow3
    qDebug() << "RUN =" << name_dir;

    QString var_path ("/storage/runs");
    QString parent_path (var_path + "/cosmic18-05/");
//    QString parent_path ("/home/plotnikov/");
    parent_path += name_dir;

    QDir dir;
    bool f_state = QDir().mkpath( parent_path );
    if (f_state){
        QProcess procSetNameFile;
        procSetNameFile.start( "sh", QStringList() << "-c" << QString("echo 'RUN=%1' > %2/vars.sh").arg(name_dir).arg(var_path));
        //procSetNameFile.start( "sh", QStringList() << "-c" << QString("echo 'RUN=%1' > %2/file_name.text").arg(name_dir).arg(parent_path));
        procSetNameFile.waitForFinished();
        procSetNameFile.close();
    }
    else {
        qDebug() << "Unable to create directory:" << parent_path;
    }
}

void MainWindow::startDAQ()
{
    qDebug() << "MainWindow::startDAQ ...";

    ssh_session s_ssh;
    s_ssh = ssh_new();
    if (s_ssh != nullptr){
        std::cout << "ssh session was created!\n";

        std::string host_name = "172.22.1.1";
        std::string user_name = "root";
        int port = 22;

        ssh_options_set(s_ssh, SSH_OPTIONS_HOST, host_name.c_str());
        ssh_options_set(s_ssh, SSH_OPTIONS_PORT, &port);
        ssh_options_set(s_ssh, SSH_OPTIONS_USER, user_name.c_str());

        std::cout << "Connecting to host " << host_name << " and port " << port << std::endl;
        int connection = ssh_connect( s_ssh );
        if (connection == SSH_OK){
            std::cout << "Connected\n";

            if (verify_host (s_ssh)){
                int rc = ssh_userauth_none(s_ssh, nullptr);
                if (rc == SSH_AUTH_SUCCESS) {
                    std::cout << "Authenticating with password: OK" << std::endl;


                    std::cout << "Channel...\n";
                    ssh_channel channel = ssh_channel_new(s_ssh);
                    if (channel != nullptr) {
                        std::cout << "Opening...\n";
                        rc = ssh_channel_open_session(channel);
                        if (rc == SSH_OK){
                            std::string command = "./daq_cosm.sh";
                            std::cout << "Executing remote command: \"" << command <<"\"" << "...\n";
                            rc = ssh_channel_request_exec(channel, command.c_str());
                            if (rc == SSH_OK){
                                std::cout << "Received:\n";
                                char buffer[1024];
                                auto nbytes {0};
                                auto start_time = std::chrono::high_resolution_clock::now();
                                do {
                                    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
                                    fwrite(buffer, 1, nbytes, stdout);
                                    std::cout.flush();

                                    auto stop_time = std::chrono::high_resolution_clock::now();
                                    std::chrono::duration<double> elapsed = stop_time - start_time;
                                    //std::cout << "Elapsed time: " << elapsed.count() << " s\n";
                                    if (elapsed.count() > 60)   break;
                                } while (nbytes > 0);
                                std::cout.flush();

                                ssh_channel_send_eof(channel);
                            }
                            ssh_channel_close( channel );
                        }
                        ssh_channel_free( channel );
                    }
                }
                else {
                    fprintf(stderr, "Error authenticating with password: %s\n", ssh_get_error(s_ssh));
                }
            }
            ssh_disconnect(s_ssh);
        }
        else {
            std::cerr << "Error connection to " << host_name << ": " << ssh_get_error(s_ssh);
        }
        ssh_free(s_ssh);
    }
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


    QString name_path("hv-scan");
    name_path += QDateTime::currentDateTime().toString("/dd.MM.yyyy_hh:mm:ss/");
    qDebug() << "path:" << name_path;


    for (auto crVolt {volStart}; crVolt <= volStop; crVolt += volStep)
    {
        // [1] set voltage
        qDebug() << "\n   Set voltage " << crVolt;
        //hvs.setVoltageSystem( crVolt );
        makeDirectory( name_path + QString::number(crVolt) );

        // [2] delay or monitoring current
        qDebug() << "   Delay ... ";
        QThread::sleep( 3 );

        // [3] data acquisition
        //      [3.1] data processing
        //      [3.2] data visualization
        qDebug() << "   Run DAQ ...";
        //startDAQ();
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
    std::cout << "start application ...\n";

    ssh_session s_ssh;
    s_ssh = ssh_new();
    if (s_ssh != nullptr){
        std::cout << "ssh session was created!\n";

        std::string host_name = "172.22.1.1";
        std::string user_name = "root";
        int port = 22;
        int verb = SSH_LOG_FUNCTIONS;

        ssh_options_set(s_ssh, SSH_OPTIONS_HOST, host_name.c_str());
        ssh_options_set(s_ssh, SSH_OPTIONS_PORT, &port);
        ssh_options_set(s_ssh, SSH_OPTIONS_USER, user_name.c_str());
        //ssh_options_set(s_ssh, SSH_OPTIONS_LOG_VERBOSITY, &verb);

        std::cout << "Connecting to host " << host_name << " and port " << port << std::endl;
        int connection = ssh_connect( s_ssh );
        if (connection == SSH_OK){
            std::cout << "Connected\n";

            if (verify_host (s_ssh)){
                int rc = ssh_userauth_none(s_ssh, nullptr);
                if (rc == SSH_AUTH_SUCCESS) {
                    std::cout << "Authenticating with password: OK" << std::endl;


                    std::cout << "Channel...\n";
                    ssh_channel channel = ssh_channel_new(s_ssh);
                    if (channel != nullptr) {
                        std::cout << "Opening...\n";
                        rc = ssh_channel_open_session(channel);
                        if (rc == SSH_OK){
                            std::string command = "./daq_cosm.sh";
                            std::cout << "Executing remote command: \"" << command <<"\"" << "...\n";
                            rc = ssh_channel_request_exec(channel, command.c_str());
                            if (rc == SSH_OK){
                                std::cout << "Received:\n";
                                char buffer[1024];
                                auto nbytes {0};
                                auto start_time = std::chrono::high_resolution_clock::now();
                                do {
                                    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
                                    fwrite(buffer, 1, nbytes, stdout);
                                    std::cout.flush();

                                    auto stop_time = std::chrono::high_resolution_clock::now();
                                    std::chrono::duration<double> elapsed = stop_time - start_time;
                                    //std::cout << "Elapsed time: " << elapsed.count() << " s\n";
                                    if (elapsed.count() > 30)   break;
                                } while (nbytes > 0);
                                std::cout.flush();

                                ssh_channel_send_eof(channel);
                            }
                            ssh_channel_close( channel );
                        }
                        ssh_channel_free( channel );
                    }
                }
                else {
                    fprintf(stderr, "Error authenticating with password: %s\n", ssh_get_error(s_ssh));
                }
            }


            ssh_disconnect(s_ssh);
        }
        else {
            std::cerr << "Error connection to " << host_name << ": " << ssh_get_error(s_ssh);
        }
        ssh_free(s_ssh);
    }


    std::cout << "stop application ...\n";
}

void MainWindow::slGetInfoChannels()
{
    qDebug() << "MainWindow::slGetInfoChannels() ...";


    hvs.getChannelParameters("VMon");
    hvs.getChannelParameters("IMon");
    hvs.getChannelParameters("Pw");

    qDebug() << "\nInfo channels:";
    for (size_t i {0}; i < nmChannels; ++i) {
        if (0){
            qDebug() << "VMon =" << hvs.arrChan[i].VMon
                     << "IMon =" << hvs.arrChan[i].IMon
                     << "State =" << hvs.arrChan[i].Pw;
        }
        lsWChannels[i].state->setChecked( hvs.arrChan[i].Pw );
        lsWChannels[i].volt->setValue( hvs.arrChan[i].VMon );
        lsWChannels[i].curr->setText( QString::number(hvs.arrChan[i].IMon) );

    }
}
