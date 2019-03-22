#ifndef HVSYSTEM_H
#define HVSYSTEM_H

#include <QObject>
#include <QDebug>

#include <cstdlib>

#include "CAENHVWrapper.h"


struct Channel{
    QString name;
    uint V0Set;
    uint I0Set;
    uint VMon;
    uint IMon;
    bool Pw;

};

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
    void setChannelParameters();

    void printActiveChannels();

    void setPowerChannel(uint8_t nm_chan, bool state);
    void setPowerSystem(bool state);
    void setVoltageChannel(uint8_t nm_chan, ulong voltage);
    void setVoltageSystem(ulong voltage);




private:
    bool f_connect;
    int  handle;

    CAENHVRESULT    ret     {-1};

    // module parameters
    static constexpr ushort     slot    {4};    // slot number
    static constexpr ushort     numChan {12};   // number of channels


    std::vector<ushort> lstChan;
    ushort  listChan [numChan];
    Channel arrChan  [numChan];

    void initSystem();

};

#endif // HV SYSTEM_H
