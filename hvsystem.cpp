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
    getChannelName();   // set name widgets


    // [3] set active channels
    getChannelParameters("Pw");
    for (ushort i {0}; i < numChan; i++)
        if (arrChan[i].Pw)
            lstActiveChan.push_back(i);

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
    emit sendMessage( QString("CAENHV_InitSystem: %1 (num. %2)").arg( CAENHV_GetError(sysHndl) ).arg( ret ) );
    qDebug() << QString("CAENHV_InitSystem: %1 (num. %2)").arg( CAENHV_GetError(sysHndl) ).arg( ret );
    if( ret == CAENHV_OK ){
        handle = sysHndl;
        initSystem();
        emit sgnLogged( true );
    }
    else {
        emit sgnLogged( false );
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
        emit sendMessage("No connection to power supply!");
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

        if (lstNmChanBoard != nullptr)
            free( lstNmChanBoard );

        if (lstSNBoard != nullptr)
            free( lstSNBoard );

        if (lstModel != nullptr)
            free( lstModel );

        if (lstDescription != nullptr)
            free( lstDescription );

        if (lstFmwRelMin != nullptr)
            free( lstFmwRelMin );

        if (lstFmwRelMax != nullptr)
            free( lstFmwRelMax );
    }
}

void HVSystem::getChannelName()
{
    qDebug() << "HVSystem::getChannelName() ...";

    if (!f_connect){
        qDebug() << "No connection to power supply!";
        emit sendMessage("No connection to power supply!");
        return;
    }


    char  listNameCh[numChan][MAX_CH_NAME];
    ret = CAENHV_GetChName(handle, slot, numChan, listChan, listNameCh);
    if( ret == CAENHV_OK )
    {
        qDebug() << "Channel name:";
        for(auto i {0}; i < numChan; i++ ){
            qDebug() << QString("   Channel n. %1: %2").arg(listChan[i]).arg(listNameCh[i]);
            arrChan[i].name = listNameCh[i];
        }
    }
    else
    {
        qDebug() << QString("CAENHV_GetChName: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret);
        return;
    }
}

void HVSystem::getChannelParameters(const std::string parName)
{
    //qDebug() << "HVSystem::getChannelParameters() ...";

    if (!f_connect){
        qDebug() << "No connection to power supply!";
        emit sendMessage("No connection to power supply!");
        return;
    }


    float *fListParVal  {nullptr};
    ulong *lListParVal  {nullptr};
    ulong type          {0};


    // [1] determine the type of the parameter value
    ret = CAENHV_GetChParamProp(handle, slot, listChan[0], parName.c_str(), "Type", &type);
    //qDebug() << "type =" << type;
    if( ret != CAENHV_OK ) {
        qDebug() << QString("CAENHV_GetChParamProp: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret);
        return;
    }

    // [2] get the value of the parameter
    if( type == PARAM_TYPE_NUMERIC ) {
        fListParVal = static_cast<float *>(malloc(sizeof(float) * numChan));
        ret = CAENHV_GetChParam(handle, slot, parName.c_str(), numChan, listChan, fListParVal);
    }
    else {
        lListParVal = static_cast<ulong *>(malloc(sizeof(long) * numChan));
        ret = CAENHV_GetChParam(handle, slot, parName.c_str(), numChan, listChan, lListParVal);
    }

    if( ret != CAENHV_OK ) {
        qDebug() << QString("CAENHV_GetChParam: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret);
    }
    else {
        for(auto i {0u}; i < numChan; i++ ){
            if (parName == "VMon"){
                arrChan[i].VMon = fListParVal[i];
            }
            else {
                if (parName == "IMon"){
                    arrChan[i].IMon = fListParVal[i];
                }
                else {
                    if (parName == "Pw"){
                        arrChan[i].Pw = lListParVal[i];
                    }
                }
            }
        }
    }

    // [3] release resources
    if (fListParVal != nullptr)
        free (fListParVal);
    if (lListParVal != nullptr)
        free (lListParVal);
}

void HVSystem::setChannelParameters()
{
    //qDebug() << "HVSystem::setChannelParameters() ...";

    if (!f_connect){
        qDebug() << "No connection to power supply!";
        emit sendMessage("No connection to power supply!");
        return;
    }


    char namePar[]      {"V0Set"};
    ulong type          {0};
    ulong lValPar       {2000};
    float fValPar       {2000.};

    ushort nmChan       {2};        // number of channels
    ushort chan[2]    = {0, 1};     // array of channel numbers

    // [1] задаем каналы, с которыми работаем
    //      1/1 enter number of channel(s)
    //      1/2 enter channel (loop)

    // [2]
    ret = CAENHV_GetChParamProp(handle, slot, chan[0], namePar, "Type", &type);
    if( ret != CAENHV_OK ){
        qDebug() << QString("CAENHV_GetChParamProp: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret);
        return;
    }

    qDebug() << QString("Value %1").arg(namePar);
    qDebug() << QString("Type of value: %1").arg(type);
    if( type == PARAM_TYPE_NUMERIC ){
        ret = CAENHV_SetChParam(handle, slot, namePar, nmChan, chan, &fValPar);
    }
    else {
        ret = CAENHV_SetChParam(handle, slot, namePar, nmChan, chan, &lValPar);
    }

    qDebug() << QString("CAENHV_SetChParam: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret);
}

void HVSystem::printActiveChannels() const
{
    qDebug() << "List of active channels:";
    qDebug() << "size =" << lstActiveChan.size();
    for(auto chan: lstActiveChan){
        qDebug() << "   channel:" << chan;
    }
}


void HVSystem::setVoltageChannel(const uint8_t nm_chan, const float voltage)
{
    qDebug() << "HVSystem::setVoltageChannel() ...";

    if (!f_connect){
        qDebug() << "No connection to power supply!";
        emit sendMessage("No connection to power supply!");
        return;
    }


    char namePar[]  {"V0Set"};
    float volt      {voltage};
    ushort nmChan   {1};            // number of channels
    ushort chan[1]  {nm_chan};      // array of channel numbers


    ret = CAENHV_SetChParam(handle, slot, namePar, nmChan, chan, &volt);
    qDebug() << QString("CAENHV_SetChParam: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret);
    if( ret != CAENHV_OK ){     // проблема!!!
        emit sendMessage(QString("CAENHV_SetChParam: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret));
    }
}

void HVSystem::setVoltageSystem(const float voltage)
{
    qDebug() << "HVSystem::setVoltageChannel() ...";

    if (!f_connect){
        qDebug() << "No connection to power supply!";
        emit sendMessage("No connection to power supply!");
        return;
    }


    char namePar[]  {"V0Set"};
    float volt      {voltage};
    ushort nmChan   {static_cast<ushort>(lstActiveChan.size())};          // number of channels
    std::vector<ushort> chan( lstActiveChan.size() );
    std::copy(lstActiveChan.cbegin(), lstActiveChan.cend(), chan.begin());

    ret = CAENHV_SetChParam(handle, slot, namePar, nmChan, chan.data(), &volt);
    qDebug() << QString("CAENHV_SetChParam: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret);

    if( ret != CAENHV_OK ){     // проблема!!!
        emit sendMessage(QString("CAENHV_SetChParam: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret));
    }
}

void HVSystem::setPowerChannel(uint8_t nm_chan, bool state)
{
    qDebug() << "HVSystem::setStateChannel() ...";

    if (!f_connect){
        qDebug() << "No connection to power supply!";
        emit sendMessage("No connection to power supply!");
        return;
    }

    char namePar[]  {"Pw"};
    ulong pw_state  {state};
    ushort nmChan   {1};        // number of channels
    ushort chan[1]  {nm_chan};  // array of channel numbers


    ret = CAENHV_SetChParam(handle, slot, namePar, nmChan, chan, &pw_state);
    qDebug() << QString("CAENHV_SetChParam: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret);
    if( ret == CAENHV_OK ){
        arrChan[nm_chan].Pw = state;
        if (state){
            lstActiveChan.push_back(nm_chan);
        }
        else {
            lstActiveChan.remove(nm_chan);
        }
    }
    else {
        emit sendMessage(QString("CAENHV_SetChParam: %1 (num. %2)").arg(CAENHV_GetError(handle)).arg(ret));
    }
}

void HVSystem::setPowerSystem(bool state)
{
    // включаем все необходимые каналы
    // будет использоваться при hv-scan
    // будем использовать при настроенной опции в приложении
    // чтобы запустил ПО и активировались сразу все необходимые каналы
    // при выходи из приложения запоминать рабочие каналы

    Q_UNUSED(state);

}
