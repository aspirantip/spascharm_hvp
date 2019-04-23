#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QThread>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QDebug>
#include <QProcess>
#include <QDir>
#include <QDateTime>


// std
#include <array>
#include <chrono>
#include <iostream>
#include <thread>


#include "hvsystem.h"


#include <libssh/libssh.h>


#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


struct wdgChannel{
    QCheckBox   *state  {nullptr};
    QSpinBox    *volt   {nullptr};
    QLabel      *curr   {nullptr};
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void slStartHVScan();
    void slChangeStateChannel();
    void slChangeVoltChannel(int value);
    void slSetNamesChannels();
    void slGetInfoChannels();

    void slConnectHVP(bool state);
    void startDAQ();

    void slTest();

private:
    Ui::MainWindow *ui;
    HVSystem hvs;
    QTimer timer;
    QTimer tmrInfoChannel;


    static constexpr uint8_t nmChannels {12};
    std::array<wdgChannel, nmChannels> lsWChannels;

    void initGUI();
    void createConnections();
    void makeDirectory(QString name_dir);
    void hvscan();
    //void startDAQ();
    static void tmr_info();
};

#endif // MAINWINDOW_H
