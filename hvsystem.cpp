#include "hvsystem.h"

HVSystem::HVSystem()
{
    qDebug() << "HVSystem::HVSystem() ...";


}

HVSystem::~HVSystem()
{

}

void HVSystem::Login()
{
    qDebug() << "HVSystem::Login()";

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

    //return;

    ret = CAENHV_InitSystem(sysType, linkType, ip_addr, username, password, &sysHndl);

    qDebug() << QString("CAENHV_InitSystem: %1 (num. %2)").arg( CAENHV_GetError(sysHndl) ).arg( ret );
    if( ret == CAENHV_OK )
    {

    }

}
