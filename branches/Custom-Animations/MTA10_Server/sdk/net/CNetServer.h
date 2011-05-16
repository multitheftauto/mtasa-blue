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
    enum ENetworkUsageDirection
    {
        STATS_INCOMING_TRAFFIC = 0,
        STATS_OUTGOING_TRAFFIC = 1
    };

    // szIP can be NULL if autochoosing is wanted.
    virtual bool                            StartNetwork                    ( const char* szIP, unsigned short usServerPort, unsigned int uiAllowedPlayers ) = 0;
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

    virtual void                            GetNetworkUsageData             ( CNetServer::ENetworkUsageDirection, unsigned long ulBits[256], unsigned long ulCount[256] ) = 0;

    virtual void                            Kick                            ( NetServerPlayerID &PlayerID ) = 0;

    virtual void                            SetPassword                     ( const char* szPassword ) = 0;

    virtual void                            SetMaximumIncomingConnections   ( unsigned short numberAllowed ) = 0;

    virtual CNetHTTPDownloadManagerInterface*   GetHTTPDownloadManager      ( void ) = 0;

    virtual void                            SetClientBitStreamVersion       ( const NetServerPlayerID &PlayerID, unsigned short usBitStreamVersion ) = 0;
    virtual void                            ClearClientBitStreamVersion     ( const NetServerPlayerID &PlayerID ) = 0;
    virtual unsigned short                  GetClientBitStreamVersion       ( const NetServerPlayerID &PlayerID ) = 0;

    virtual void                            EnablePortForwarding            ( unsigned short usPort, const char* szProtocol, bool bEnable ) = 0;
    virtual int                             GetPortForwardingStatus         ( unsigned short usPort, const char* szProtocol ) = 0;  // 0:closed  1:busy  2:open  -1:fail

    virtual void                            ResetStub                       ( uint dwType, ... ) = 0;
    virtual void                            ResetStub                       ( uint dwType, va_list ) = 0;
};

#endif

