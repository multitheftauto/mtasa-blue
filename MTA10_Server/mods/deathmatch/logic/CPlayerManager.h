/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPlayerManager.h
*  PURPOSE:     Player ped entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CPlayerManager;

#ifndef __CPLAYERMANAGER_H
#define __CPLAYERMANAGER_H

#include "CCommon.h"
#include "packets/CPacket.h"
#include "CPlayer.h"
#include "../Config.h"

class CPlayerManager
{
    friend class CPlayer;

public:
                                                CPlayerManager                  ( void );
                                                ~CPlayerManager                 ( void );

    void                                        DoPulse                         ( void );

    inline void                                 SetScriptDebugging              ( class CScriptDebugging* pScriptDebugging )        { m_pScriptDebugging = pScriptDebugging; };

    CPlayer*                                    Create                          ( const NetServerPlayerID& PlayerSocket );
    void                                        DeleteAll                       ( void );

    inline unsigned int                         Count                           ( void )                                            { return static_cast < unsigned int > ( m_Players.size () ); ; };
    inline unsigned int                         CountJoined                     ( void )                                            { return CountWithStatus ( STATUS_JOINED ); };
    unsigned int                                CountWithStatus                 ( int iStatus );
    bool                                        Exists                          ( CPlayer* pPlayer );

    CPlayer*                                    Get                             ( NetServerPlayerID& PlayerSocket );
    CPlayer*                                    Get                             ( const char* szNick, bool bCaseSensitive = false );

    CPlayer*                                    GetBefore                       ( ElementID PlayerID );
    CPlayer*                                    GetAfter                        ( ElementID PlayerID );

    inline std::list < CPlayer* > ::const_iterator  IterBegin                   ( void )                                            { return m_Players.begin (); };
    inline std::list < CPlayer* > ::const_iterator  IterEnd                     ( void )                                            { return m_Players.end (); };
    inline std::list < CPlayer* > ::const_iterator  IterGet                     ( CPlayer* pPlayer );
    inline std::list < CPlayer* > ::const_iterator  IterGet                     ( ElementID PlayerID );

    void                                        Broadcast                       ( const CPacket& Packet, CPlayer* pSkip = NULL );
    void                                        Broadcast                       ( const CPacket& Packet, std::list < CPlayer * > & playersList );
    void                                        BroadcastOnlyJoined             ( const CPacket& Packet, CPlayer* pSkip = NULL, NetServerPacketOrdering packetOrdering = PACKET_ORDERING_GAME );

    static bool                                 IsValidPlayerModel              ( unsigned short usPlayerModel );

    void                                        ResetAll                        ( void );

    void                                        ClearSyncTime                   ( CPlayer& Player );
    void                                        ClearSyncTimes                  ( void );

private:
    inline void                                 AddToList                       ( CPlayer* pPlayer )                                { m_Players.push_back ( pPlayer ); };
    void                                        RemoveFromList                  ( CPlayer* pPlayer );

    class CScriptDebugging*                     m_pScriptDebugging;

    std::list < CPlayer* >                      m_Players;
    bool                                        m_bCanRemoveFromList;
};

#endif
