#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

private:
    Ui::MainWindow *ui;
    HVSystem hvs;
};

#endif // MAINWINDOW_H
