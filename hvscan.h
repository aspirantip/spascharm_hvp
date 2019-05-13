#ifndef HVSCAN_H
#define HVSCAN_H

#include <QObject>
#include <QThread>
#include <QDateTime>
#include <QDir>
#include <QProcess>

#include <libssh/libssh.h>

#include <iostream>

#include "hvsystem.h"

class HVScan : public QThread
{
    Q_OBJECT

public:
    explicit HVScan(QObject *parent = nullptr);
    ~HVScan() override;

    void run() override;
    void setVoltageRange(const uint16_t start, const uint16_t stop);
    void setVoltageStep (const uint8_t step);
    void setTime (const int hvs_time);
    void setHVPower (HVSystem* power);
    void stopHVScan();

private:
    int v_start;
    int v_stop;
    int v_step;
    int time;
    HVSystem* hv_power;
    volatile bool f_run;


    void makeDirectory(const QString name_dir);
    void startDAQ();
    bool verify_host(ssh_session session);

};

#endif // HVSCAN_H
