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
        f_connect = true;
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
        for(auto i {0}; i < nmSlots; i++){
            QString str (QString("Board %1: ").arg(i));
            if( *lstModel == '\0' ){
                qDebug() << str.append("Not Present");
            }
            else{
                str += QString("%1 %2 ").arg(lstModel).arg(lstDescription);
                str += QString("Channels: %1  SN: %2   ").arg(lstNmChanBoard[i]).arg(lstSNBoard[i]);
                str += QString("Rel. %1.%2").arg(lstFmwRelMin[i]).arg(lstFmwRelMax[i]);
                qDebug() << str;
            }

            lstModel += strlen(lstModel) + 1;
            lstDescription += strlen(lstDescription) + 1;
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


    CAENHVRESULT    ret     {-1};
    ushort          slot    {0};    // slot number
    const ushort    numChan {12};   // number of channels
    ushort          listChan [numChan];
    //unsigned short listChan [2048];
    //char  (*listNameCh)[MAX_CH_NAME];
    char  listNameCh[numChan][MAX_CH_NAME];     // из-за этого могут быть проблемы?


    for (ushort i {0}; i < numChan; i++)
    {
        listChan[i] = i;
    }

    //listNameCh = (char *) malloc(numChan * MAX_CH_NAME);

    /*
    for (auto i {0}; i < numChan; i++)
        listNameCh = (char *) malloc(MAX_CH_NAME);
    */

    ret = CAENHV_GetChName(handle, slot, numChan, listChan, listNameCh);
    if( ret != CAENHV_OK )
    {
        //free(listNameCh);
        qDebug() << QString("CAENHV_GetChName: %1 (num. %2)\n").arg(CAENHV_GetError(handle)).arg(ret);
        return;
    }
    else
    {
        qDebug() << "Channel name:";
        for(auto i {0}; i < numChan; i++ )
            qDebug() << QString("Channel n. %1: %1\n").arg(listChan[i]).arg(listNameCh[i]);
    }


    /* per quando era definito come un array di puntatori a char
    for( n = 0; n < NrOfCh; n++)
        free(listNameCh[n]);
    */
    //free(listNameCh);
}

void HVSystem::getChannelParameters()
{

}

void HVSystem::setChannelParameters()
{

}
