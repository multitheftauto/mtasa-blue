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

#include <CClientCommon.h>
#include "CDeathmatchObject.h"

#ifdef WITH_OBJECT_SYNC

class CObjectSync
{
public:
                                    CObjectSync                         ( CClientObjectManager* pObjectManager );

    bool                            ProcessPacket                       ( unsigned char ucPacketID, NetBitStreamInterface& bitStream );
    void                            DoPulse                             ( void );

    void                            AddObject                           ( CDeathmatchObject* pObject );
    void                            RemoveObject                        ( CDeathmatchObject* pObject );
    void                            ClearObjects                        ( void );

    inline std::list < CDeathmatchObject* > ::const_iterator
                                    IterBegin                           ( void )    { return m_List.begin (); };
    inline std::list < CDeathmatchObject* > ::const_iterator
                                    IterEnd                             ( void )    { return m_List.end (); };

    bool                            Exists                              ( CDeathmatchObject* pObject );
private:
    void                            Packet_ObjectStartSync              ( NetBitStreamInterface& BitStream );
    void                            Packet_ObjectStopSync               ( NetBitStreamInterface& BitStream );
    void                            Packet_ObjectSync                   ( NetBitStreamInterface& BitStream );

    void                            Sync                                ( void );
    void                            WriteObjectInformation              ( NetBitStreamInterface* pBitStream, CDeathmatchObject* pObject );

    CClientObjectManager*           m_pObjectManager;
    CMappedList < CDeathmatchObject* > m_List;
    unsigned long                   m_ulLastSyncTime;
};

#endif

#endif
