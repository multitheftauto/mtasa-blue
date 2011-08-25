/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CObjectSync.h
*  PURPOSE:     Header for object sync class
*  DEVELOPERS:  Stanislav Izmalkov <izstas@live.ru>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __COBJECTSYNC_H
#define __COBJECTSYNC_H

#include "CPlayerManager.h"
#include "CObjectManager.h"
#include "packets/CObjectSyncPacket.h"

class CObjectSync
{
public:
                            CObjectSync                         ( CPlayerManager* pPlayerManager, CObjectManager* pObjectManager );

    void                    DoPulse                             ( void );
    bool                    ProcessPacket                       ( CPacket& Packet );

    void                    OverrideSyncer                      ( CObject* pObject, CPlayer* pPlayer );

private:
    void                    Update                              ( void );
    void                    UpdateObject                        ( CObject* pObject );
    void                    FindSyncer                          ( CObject* pObject );
    CPlayer*                FindPlayerCloseToObject             ( CObject* pObject, float fMaxDistance );

    void                    StartSync                           ( CPlayer* pPlayer, CObject* pObject );
    void                    StopSync                            ( CObject* pObject );

    void                    Packet_ObjectSync                   ( CObjectSyncPacket& Packet );

    CPlayerManager*         m_pPlayerManager;
    CObjectManager*         m_pObjectManager;

    unsigned long           m_ulLastSweepTime;
};

#endif
