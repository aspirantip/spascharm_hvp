#include "hvscan.h"

HVScan::HVScan(QObject *parent)
{
    time    = 0;
    events  = 0;
}

HVScan::~HVScan()
{

}

void HVScan::run()
{
    qDebug() << "\n===============================";
    qDebug() << "Start HVScan ...";

    QString name_path("hv-scan");
    name_path += QDateTime::currentDateTime().toString("/dd.MM.yyyy_hh:mm:ss/");
    qDebug() << "path:" << name_path;



    f_run = true;

    if (v_stop < v_start)
    {
        // start voltage setting function
        hv_power->setVoltageSystem( v_stop );
        std::this_thread::sleep_for (std::chrono::seconds(5));
        for (auto crVolt {v_start}; crVolt >= v_stop; crVolt -= v_step)
        {
            // [1] set voltage
            if (!f_run)
                break;
            qDebug() << "\n   Set voltage " << crVolt;
            hv_power->setVoltageSystem( crVolt );
            makeDirectory( name_path + QString::number(crVolt) );

            // [2] delay or monitoring current
            if (!f_run)
                break;
            qDebug() << "   Delay ... ";
            QThread::sleep( 10 );

            // [2.1] проверяем текущее напряжение, оно должно соответствовать задаваемому +-порог
            if (!waitVoltage( crVolt )){
                stopHVScan();
                emit sgnSendMessage(QString("HV-scan has been stopped: voltage (%1) not set").arg(crVolt));
            }
            // [2.2] проверям ток в каналах /    +-порог
            //waitCurrent(0);

            // [3] data acquisition
            if (!f_run)
                break;
            qDebug() << "   Run DAQ ...";
            startDAQ();
        }
    }
    else {
        // start voltage setting function
        //hv_power->setVoltageSystem( v_start );
        //std::this_thread::sleep_for (std::chrono::seconds(5));
        for (auto crVolt {v_start}; crVolt <= v_stop; crVolt += v_step)
        {
            // [1] set voltage
            if (!f_run) break;
            qDebug() << "\n   Set voltage " << crVolt;
            hv_power->setVoltageSystem( crVolt );

            // [2] delay or monitoring current
            if (!f_run) break;
            qDebug() << "   Waiting for voltage setting ... ";

            // [2.1] проверяем текущее напряжение, оно должно соответствовать задаваемому +-порог
            if (!waitVoltage( crVolt )){
                stopHVScan();
                emit sgnSendMessage(QString("HV-scan has been stopped: voltage (%1) not set").arg(crVolt));
            }
            // [2.2] проверям ток в каналах /    +-порог
            if ( waitCurrent() ){
                stopHVScan();
                emit sgnSendMessage(QString("HV-scan has been stopped: over current threshold"));
            }
            //QThread::sleep( 10 );

            // [3] make directory for data
            makeDirectory( name_path + QString::number(crVolt) );

            // [4] data acquisition
            if (!f_run) break;
            qDebug() << "   Run DAQ ...";
            startDAQ();
        }
    }

    if (f_run){
        emit sgnSendMessage("HV-scan was completed succesfully.");
    }

    qDebug() << "Stop HVScan ...";
    qDebug() << "===============================";
}

void HVScan::setVoltageRange(const uint16_t start, const uint16_t stop)
{
    v_start = start;
    v_stop  = stop;
}

void HVScan::setVoltageStep(const uint8_t step)
{
    v_step = step;
}

void HVScan::setTime(const int hvs_time)
{
    time = hvs_time;
}

void HVScan::setNumberEvents(const int nmEvents)
{
    events = nmEvents;
}

void HVScan::setHVPower(HVSystem *power)
{
    hv_power = power;
}

void HVScan::stopHVScan()
{
    f_run = false;
    qDebug() << "HVScan::stopHVScan ...";
    qDebug() << "f_run =" << f_run;
}

void HVScan::makeDirectory(const QString name_dir)
{
    qDebug() << "RUN =" << name_dir;

    bool f_test {false};

    QString var_path ("/storage/runs");
    QString parent_path;
    if (f_test){
        parent_path = "/home/plotnikov/";
    }
    else {
        parent_path = var_path + "/cosmic18-05/";
    }
//    QString parent_path (var_path + "/cosmic18-05/");
//    QString parent_path ("/home/plotnikov/");
    parent_path += name_dir;

    QDir dir;
    bool f_state = QDir().mkpath( parent_path );
    if (f_state){
        QProcess procSetNameFile;
        if (f_test){
            procSetNameFile.start( "sh", QStringList() << "-c" << QString("echo 'RUN=%1' > %2/file_name.text").arg(name_dir).arg(parent_path));
        }
        else {
            procSetNameFile.start( "sh", QStringList() << "-c" << QString("echo 'RUN=%1' > %2/vars.sh").arg(name_dir).arg(var_path));
        }
        procSetNameFile.waitForFinished();
        procSetNameFile.close();
    }
    else {
        qDebug() << "Unable to create directory:" << parent_path;
    }

}

void HVScan::startDAQ()
{
    qDebug() << "HVScan::startDAQ ...";

    ssh_session s_ssh;
    s_ssh = ssh_new();
    if (s_ssh != nullptr){
        qDebug() << "ssh session was created!\n";

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
                                std::chrono::duration<double> elapsed;
                                do {
                                    //std::cout << "wait data ...\n" << std::endl;
                                    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
                                    fwrite(buffer, 1, nbytes, stdout);
                                    //std::cout << "read data: " << nbytes << " bytes\n";
                                    std::cout.flush();

                                    auto stop_time = std::chrono::high_resolution_clock::now();
                                    elapsed = stop_time - start_time;
                                    std::cout << "Elapsed time: " << elapsed.count() << " s\n";
                                    if (elapsed.count() > time)   break;
                                } while (nbytes > 0 || elapsed.count() < time );
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

    //std::cout << "\nstop DAQ" << std::endl;
}

bool HVScan::verify_host(ssh_session session)
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

bool HVScan::waitVoltage(const float volt)
{
    qDebug() << "HVScan::waitVoltage [...]";

    const float thresh       {2.0};
    const uint8_t nmChannels {12};

    std::array<float, nmChannels> lastVolt;
    lastVolt.fill(0);

    // [1] проверка на изменение значения напряжения в каналах
    bool f_setVoltage {false};
    do {
        hv_power->getChannelParameters("Pw");
        hv_power->getChannelParameters("VMon");
        for (size_t i {0}; i < nmChannels; ++i) {
            if (hv_power->arrChan[i].Pw){
                auto m_volt     {hv_power->arrChan[i].VMon};
                auto diffVolt   {lastVolt[i] - m_volt};
                qDebug() << "volt (specified|measured):" << volt << " | " << m_volt;
                if (std::abs(diffVolt) > thresh ){
                    lastVolt[i] = m_volt;
                    f_setVoltage = true;
                    break;
                }
            }
        }
        QThread::sleep(1);
    } while( f_setVoltage );

    // [2] проверка на превышение разности значений заданного напряжения и измеренного
    uint8_t cnt_check       {0};
    const uint8_t nmCheck   {5};
    do {
        hv_power->getChannelParameters("Pw");
        hv_power->getChannelParameters("VMon");
        f_setVoltage = true;
        for (size_t i {0}; i < nmChannels; ++i) {
            if (hv_power->arrChan[i].Pw){
                auto m_volt = hv_power->arrChan[i].VMon;
                qDebug() << "volt (specified|measured):" << volt << " | " << m_volt;
                if (std::abs(volt-m_volt) > thresh){
                    f_setVoltage = false;
                    break;
                }
            }
        }
        qDebug() << "f_setVoltage =" << f_setVoltage << " cnt_check =" << cnt_check;
        QThread::sleep(1);
    } while( !f_setVoltage && cnt_check++ < nmCheck );

    return f_setVoltage;
}

bool HVScan::waitCurrent()
{
    // 1) проверить, что ток стабилизировался
    // 2) проверить, что не превышаем порог

    const float thresh       {1.0};
    const uint8_t nmChannels {12};

    bool f_stabCurrent = true;
    std::array<float, nmChannels> lastCurrent;
    lastCurrent.fill(0);

    // [1] проверка на стабилизацию тока
    do {
        hv_power->getChannelParameters("Pw");
        hv_power->getChannelParameters("IMon");
        for (size_t i {0}; i < nmChannels; ++i) {
            if (hv_power->arrChan[i].Pw){
                auto m_curr = hv_power->arrChan[i].IMon;
                if (std::abs(m_curr-lastCurrent[i]) > 0.0f){
                    lastCurrent[i] = m_curr;
                    f_stabCurrent = false;
                    break;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }while(!f_stabCurrent);

    // [2] проверяем ток на превышение заданного порога

    uint8_t cnt_check       {0};
    const uint8_t nmCheck   {5};
    bool f_threshold        {false};
    do {
        hv_power->getChannelParameters("Pw");
        hv_power->getChannelParameters("VMon");
        f_threshold = false;
        for (size_t i {0}; i < nmChannels; ++i) {
            if (hv_power->arrChan[i].Pw){
                auto m_curr = hv_power->arrChan[i].IMon;
                if (std::abs(m_curr-lastCurrent[i]) > thresh){
                    f_threshold = true;
                    break;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    } while( f_threshold && cnt_check++ < nmCheck );

    return f_threshold;
}

