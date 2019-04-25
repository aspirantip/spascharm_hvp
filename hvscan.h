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
    void setHVPower (HVSystem* power);

private:
    int v_start;
    int v_stop;
    int v_step;
    HVSystem* hv_power;


    void makeDirectory(const QString name_dir);
    void startDAQ();
    bool verify_host(ssh_session session);

};

#endif // HVSCAN_H
