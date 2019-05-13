#include "hvscan.h"

HVScan::HVScan(QObject *parent)
{
    time = 10;
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

    // start voltage setting function

    f_run = true;
    for (auto crVolt {v_start}; crVolt <= v_stop; crVolt += v_step)
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
        //std::this_thread::sleep_for(std::chrono::seconds(3));
        QThread::sleep( 3 );

        // [3] data acquisition
        //      [3.1] data processing
        //      [3.2] data visualization
        if (!f_run)
            break;
        qDebug() << "   Run DAQ ...";
        startDAQ();
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

