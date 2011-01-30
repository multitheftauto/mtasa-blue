/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/net/CNetServer.h
*  PURPOSE:     Net server module interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CNETSERVER_H
#define __CNETSERVER_H

#include "ns_common.h"
#include "net/bitstream.h"
#include "ns_playerid.h"
#include "CNetHTTPDownloadManagerInterface.h"

class CNetServer
{
public:
    // szIP can be NULL if autochoosing is wanted.
    virtual bool                            StartNetwork                    ( const char* szIP, unsigned short usServerPort, unsigned int uiMTUSize, unsigned int uiAllowedPlayers ) = 0;
    virtual void                            StopNetwork                     ( void ) = 0;
    virtual void                            ResetNetwork                    ( void ) = 0;

    virtual void                            DoPulse                         ( void ) = 0;

    virtual void                            RegisterPacketHandler           ( PPACKETHANDLER pfnPacketHandler, bool bIsPrimaryPacketHandler = true ) = 0;

    virtual unsigned int                    GetBitsSent                     ( NetServerPlayerID& playerID ) = 0;
    virtual unsigned int                    GetBitsReceived                 ( NetServerPlayerID& playerID ) = 0;

    virtual int                             GetPing                         ( NetServerPlayerID& playerID ) = 0;

    virtual NetBitStreamInterface*          AllocateNetServerBitStream      ( unsigned short usBitStreamVersion ) = 0;
    virtual void                            DeallocateNetServerBitStream    ( NetBitStreamInterface* bitStream ) = 0;
    virtual bool                            SendPacket                      ( unsigned char ucPacketID, NetServerPlayerID& playerID, NetBitStreamInterface* bitStream, bool bBroadcast = false, NetServerPacketPriority packetPriority = PACKET_PRIORITY_LOW, NetServerPacketReliability packetReliability = PACKET_RELIABILITY_RELIABLE, NetServerPacketOrdering packetOrdering = PACKET_ORDERING_GAME ) = 0;

    virtual void                            GetPlayerIP                     ( NetServerPlayerID& playerID, char strIP[22], unsigned short* usPort ) = 0;

    virtual void                            AddBan                          ( const char* szIP ) = 0;
    virtual void                            RemoveBan                       ( const char* szIP ) = 0;
    virtual bool                            IsBanned                        ( const char* szIP ) = 0;

    virtual void                            GetPacketLogData                ( unsigned long* ulBits, unsigned long* ulCount ) = 0;

    virtual void                            Kick                            ( NetServerPlayerID &PlayerID ) = 0;

    virtual void                            SetPassword                     ( char* szPassword ) = 0;

    virtual void                            SetMaximumIncomingConnections   ( unsigned short numberAllowed ) = 0;

    virtual bool                            AutoPatcherAddFile              ( char* szFile ) = 0;
    virtual void                            SetAutoPatcherDirectory         ( char* szDirectory ) = 0;
    virtual bool                            AutoPatcherRemoveFile           ( char* szFile ) = 0;

    virtual CNetHTTPDownloadManagerInterface*   GetHTTPDownloadManager      ( void ) = 0;

    virtual void                            SetClientBitStreamVersion       ( const NetServerPlayerID &PlayerID, unsigned short usBitStreamVersion ) = 0;
    virtual void                            ClearClientBitStreamVersion     ( const NetServerPlayerID &PlayerID ) = 0;
    virtual unsigned short                  GetClientBitStreamVersion       ( const NetServerPlayerID &PlayerID ) = 0;

    virtual void                            ResetStub                       ( DWORD dwType, ... ) = 0;
    virtual void                            ResetStub                       ( DWORD dwType, va_list ) = 0;
};

#endif

