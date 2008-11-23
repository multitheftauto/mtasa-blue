/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CConnectHistory.h
*  PURPOSE:     Client connection history class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CCONNECTHISTORY_H
#define __CCONNECTHISTORY_H

#include <list>
using namespace std;

struct CConnectHistoryItem
{
    unsigned long   ulIP;
    unsigned long   ulTime;
    unsigned char   ucCounter;
};

class CConnectHistory
{
public:
                                    CConnectHistory         ( void );
                                    ~CConnectHistory        ( void );

    bool                            IsFlooding              ( unsigned long ulIP );

    CConnectHistoryItem*            Find                    ( unsigned long ulIP );
    void                            Reset                   ( void );

private:
    void                            RemoveExpired           ( void );

    list < CConnectHistoryItem* >   m_List;
};

#endif
