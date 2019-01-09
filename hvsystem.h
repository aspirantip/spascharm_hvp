#ifndef HVSYSTEM_H
#define HVSYSTEM_H

#include <QObject>
#include <QDebug>

#include "CAENHVWrapper.h"

class HVSystem : public QObject
{
    Q_OBJECT
public:
    explicit HVSystem(QObject *parent = nullptr);
    ~HVSystem();

signals:

public slots:
    void Login();
    void Logout();
    void getCrateMap();
    void getChannelName();
    void getChannelParameters();




private:
    bool f_connect;
    int  handle;

};

#endif // HV SYSTEM_H
