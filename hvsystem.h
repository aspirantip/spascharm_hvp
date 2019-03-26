#ifndef HVSYSTEM_H
#define HVSYSTEM_H

#include <QObject>
#include <QDebug>

#include <cstdlib>
#include <algorithm>

#include "CAENHVWrapper.h"


struct Channel{
    QString name    {"name channel"};
    float V0Set     {0};
    float I0Set     {0};
    float VMon      {0};
    float IMon      {0};
    bool Pw         {false};

};

class HVSystem : public QObject
{
    Q_OBJECT
public:
    explicit HVSystem(QObject *parent = nullptr);
    ~HVSystem();



signals:
    void sgnLogged(bool state);

public slots:
    void Login();
    void Logout();
    void getCrateMap();
    void getChannelName();
    void getChannelParameters(std::string parName);
    void setChannelParameters();

    void printActiveChannels() const;

    void setPowerChannel(uint8_t nm_chan, bool state);
    void setPowerSystem(bool state);
    void setVoltageChannel(uint8_t nm_chan, ulong voltage);
    void setVoltageSystem(float voltage);





private:
    bool f_connect;
    int  handle;

    CAENHVRESULT    ret     {-1};

    // module parameters
    static constexpr ushort     slot    {4};    // slot number
    static constexpr ushort     numChan {12};   // number of channels


    std::list<ushort> lstChan;      // list of active channels (index)
    std::vector<ushort> vChan;
    ushort  listChan [numChan];


    void initSystem();


public:
    Channel arrChan  [numChan];

};

#endif // HV SYSTEM_H
