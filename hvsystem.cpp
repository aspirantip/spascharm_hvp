#include "hvsystem.h"

HVSystem::HVSystem(QObject *parent) : QObject(parent)
{
    qDebug() << "HVSystem::HVSystem() ...";
    f_connect = false;

}

HVSystem::~HVSystem()
{
    if ( f_connect )
        Logout();

}

void HVSystem::initSystem()
{
    qDebug() << "HVSystem::initSystem() ...";


    f_connect = true;

    // [1] channel number initialization
    for (ushort i {0}; i < numChan; i++)
        listChan[i] = i;

    // [2] getting channel names
    // getChannelName ();

    // [3] ...

}

void HVSystem::Login()
{
    qDebug() << "HVSystem::Login() ...";

    CAENHV_SYSTEM_TYPE_t sysType    {SY1527};
    auto linkType                   {LINKTYPE_TCPIP};
    auto username                   {"admin"};
    auto password                   {"admin"};
    char ip_addr [30]               {"172.22.4.1"};
    auto sysHndl                    {-1};
    CAENHVRESULT ret                {-1};

    qDebug() << "\tuser:      " << username;
    qDebug() << "\tpassword:  " << password;
    qDebug() << "\tip-address:" << ip_addr;


    ret = CAENHV_InitSystem(sysType, linkType, ip_addr, username, password, &sysHndl);

    qDebug() << QString("CAENHV_InitSystem: %1 (num. %2)").arg( CAENHV_GetError(sysHndl) ).arg( ret );
    if( ret == CAENHV_OK ){
        handle = sysHndl;
        initSystem();
    }
}

void HVSystem::Logout()
{
    qDebug() << "HVSystem::Logout() ...";

    if ( f_connect ){
        CAENHVRESULT ret = CAENHV_DeinitSystem (handle);
        if ( ret == CAENHV_OK )
            qDebug() << QString("CAENHV_DeinitSystem: Connection closed (num. %1)").arg(ret);
        else
            qDebug() << QString("CAENHV_DeinitSystem: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret);
    }
}

void HVSystem::getCrateMap()
{
    qDebug() << "HVSystem::getCrateMap() ...";

    if (!f_connect){
        qDebug() << "No connection to power supply!";
        return;
    }

    CAENHVRESULT ret {-1};
    ushort  nmSlots         {0};
    ushort  *lstNmChanBoard {nullptr};      // list | number of channels in each module
    ushort  *lstSNBoard     {nullptr};      // Board Serial number
    char    *lstModel       {nullptr};      // Model of the board, i.e. "A1734"
    char    *lstDescription {nullptr};      // Description of the board, i.e. "12 channels ..."
    uchar   *lstFmwRelMin   {nullptr};
    uchar   *lstFmwRelMax   {nullptr};

    ret = CAENHV_GetCrateMap(handle, &nmSlots, &lstNmChanBoard, &lstModel, &lstDescription,
                             &lstSNBoard, &lstFmwRelMin, &lstFmwRelMax );
    if (ret != CAENHV_OK ){
        QString msg (QString("CAENHV_GetCrateMap: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret));
        qDebug() << msg;
    }
    else {
        char *model = lstModel;
        char *descr = lstDescription;
        for(auto i {0}; i < nmSlots; i++){
            QString str (QString("Board %1: ").arg(i));
            if( *model == '\0' ){
                qDebug() << str.append("Not Present");
            }
            else{
                str += QString("%1 %2 ").arg(model).arg(descr);
                str += QString("Channels: %1  SN: %2   ").arg(lstNmChanBoard[i]).arg(lstSNBoard[i]);
                str += QString("Rel. %1.%2").arg(lstFmwRelMin[i]).arg(lstFmwRelMax[i]);
                qDebug() << str;
            }

            model += strlen(model) + 1;
            descr += strlen(descr) + 1;
        }

        qDebug() << "lstNmChanBoard =" << lstNmChanBoard;
        if (lstNmChanBoard != nullptr)
            free( lstNmChanBoard );

        qDebug() << "lstSNBoard     =" << lstSNBoard;
        if (lstSNBoard != nullptr)
            free( lstSNBoard );

        qDebug() << "lstModel       =" << lstModel;
        if (lstModel != nullptr)
            free( lstModel );

        qDebug() << "lstDescription =" << lstDescription;
        if (lstDescription != nullptr)
            free( lstDescription );

        qDebug() << "lstFmwRelMin   =" << lstFmwRelMin;
        if (lstFmwRelMin != nullptr)
            free( lstFmwRelMin );

        qDebug() << "lstFmwRelMax   =" << lstFmwRelMax;
        if (lstFmwRelMax != nullptr)
            free( lstFmwRelMax );
    }

}

void HVSystem::getChannelName()
{
    qDebug() << "HVSystem::getChannelName() ...";

    if (!f_connect){
        qDebug() << "No connection to power supply!";
        return;
    }


    char  listNameCh[numChan][MAX_CH_NAME];     // из-за этого могут быть проблемы?
    //listNameCh = (char *) malloc(numChan * MAX_CH_NAME);


    ret = CAENHV_GetChName(handle, slot, numChan, listChan, listNameCh);
    if( ret != CAENHV_OK )
    {
        //free(listNameCh);
        qDebug() << QString("CAENHV_GetChName: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret);
        return;
    }
    else
    {
        qDebug() << "Channel name:";
        for(auto i {0}; i < numChan; i++ )
            qDebug() << QString("   Channel n. %1: %2").arg(listChan[i]).arg(listNameCh[i]);
    }

}

void HVSystem::getChannelParameters()
{
    qDebug() << "HVSystem::getChannelParameters() ...";

    if (!f_connect){
        qDebug() << "No connection to power supply!";
        return;
    }


//    char namePar[]      {"V0Set"};       // узнать правильное значение !!!
    char namePar[]      {"VMon"};
//    char namePar[]      {"Pw"};   // состояние источника on/off - не сработало, видимо значение bool-типа
    float *fListParVal  {nullptr};
    ulong *lListParVal  {nullptr};
    ulong type          {0};


    // [1] determine the type of the parameter value
    // для конкретных функций (установка напряжения) можно пропостить этот этап
    // так как тип данных не меняется и соответственно выяснять его постоянно не нужно
    ret = CAENHV_GetChParamProp(handle, slot, listChan[0], namePar, "Type", &type);
    if( ret != CAENHV_OK )
    {
        qDebug() << QString("CAENHV_GetChParamProp: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret);
        return;
    }

    // [2] get the value of the parameter
    if( type == PARAM_TYPE_NUMERIC )
    {
        fListParVal = static_cast<float *>(malloc(sizeof(float) * numChan));
        ret = CAENHV_GetChParam(handle, slot, namePar, numChan, listChan, fListParVal);
    }
    else
    {
        lListParVal = static_cast<ulong *>(malloc(sizeof(long) * numChan));
        ret = CAENHV_GetChParam(handle, slot, namePar, numChan, listChan, lListParVal);
    }

    if( ret != CAENHV_OK )
        qDebug() << QString("CAENHV_GetChParam: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret);
    else
    {
        qDebug() << "\nParameter value:";
        for(auto i {0}; i < numChan; i++ )
            qDebug() << QString("Slot: %1  Ch: %2  %3: %4").arg(slot).arg(listChan[i]).arg(namePar).arg(fListParVal[i]);
    }

    // [3] release resources
    if (fListParVal != nullptr)
        free (fListParVal);
    if (lListParVal != nullptr)
        free (lListParVal);

}

void HVSystem::setChannelParameters()
{
    // задавать напряжение надо не вовсех каналах!
    // устанавливать значение можно в одном канале, а можно сразу во всех

    qDebug() << "HVSystem::setChannelParameters() ...";

    if (!f_connect){
        qDebug() << "No connection to power supply!";
        return;
    }


    char namePar[]{"V0Set"};
    ulong type          {0};
    ulong lValPar       {1000};
    float fValPar       {1000.};

    ushort nmChan       {1};    // number of channels
    ushort chan[1]      {4};    // array of channel numbers

    // [1] задаем каналы, с которыми работаем
    //      1/1 enter number of channel(s)
    //      1/2 enter channel (loop)

    // [2]
    ret = CAENHV_GetChParamProp(handle, slot, listChan[0], namePar, "Type", &type);
    if( ret != CAENHV_OK )
    {
        qDebug() << QString("CAENHV_GetChParamProp: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret);
        return;
    }

    qDebug() << QString("Value %1").arg(namePar);
    if( type == PARAM_TYPE_NUMERIC ){
        ret = CAENHV_SetChParam(handle, slot, namePar, nmChan, chan, &fValPar);
    }
    else {
        ret = CAENHV_SetChParam(handle, slot, namePar, nmChan, chan, &lValPar);
    }

    qDebug() << QString("CAENHV_SetChParam: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret);


}
