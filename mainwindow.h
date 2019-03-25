#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QThread>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QDebug>

// std
#include <array>

#include "hvsystem.h"


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

    void slConnectHVP(bool state);

    void slTest();

private:
    Ui::MainWindow *ui;
    HVSystem hvs;
    QTimer timer;

    QVector<QCheckBox*> vCheckBoxChannels;
    static constexpr uint8_t nmChannels {12};


    void createConnections();
};

#endif // MAINWINDOW_H
