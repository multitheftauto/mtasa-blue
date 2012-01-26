/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/net/cnet.h
*  PURPOSE:     Network subsystem interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CNET_H
#define __CNET_H

#include <string>
#include "net_common.h"
#include "net/bitstream.h"
#include "CNetHTTPDownloadManagerInterface.h"

struct SPacketStat
{
    int iCount;
    int iTotalBytes;
    TIMEUS totalTime;
};

class CNet
{
public:
    enum ENetworkUsageDirection
    {
        STATS_INCOMING_TRAFFIC = 0,
        STATS_OUTGOING_TRAFFIC = 1
    };

    virtual void                        ResetNetwork                ( void ) = 0;
    virtual bool                        StartNetwork                ( const char* szServerHost, unsigned short usServerPort, const char* szServerPassword = NULL ) = 0;
    virtual void                        StopNetwork                 ( void ) = 0;

    virtual void                        SetFakeLag                  ( unsigned short usPacketLoss, unsigned short usMinExtraPing, unsigned short usExtraPingVariance ) = 0;

    virtual bool                        IsConnected                 ( void ) = 0;

    virtual void                        DoPulse                     ( void ) = 0;

    virtual void                        RegisterPacketHandler       ( PPACKETHANDLER pfnPacketHandler ) = 0;

    virtual NetBitStreamInterface*      AllocateNetBitStream        ( void ) = 0;
    virtual void                        DeallocateNetBitStream      ( NetBitStreamInterface* bitStream ) = 0;
    virtual bool                        SendPacket                  ( unsigned char ucPacketID, NetBitStreamInterface* bitStream, NetPacketPriority packetPriority = PACKET_PRIORITY_HIGH, NetPacketReliability packetReliability = PACKET_RELIABILITY_RELIABLE_ORDERED, ePacketOrdering packetOrdering = PACKET_ORDERING_DEFAULT ) = 0;

    virtual void                        SetClientPort               ( unsigned short usClientPort ) = 0;
    virtual const char *                GetConnectedServer          ( void )=0;

    virtual bool                        GetNetworkStatistics        ( NetStatistics* pDest ) = 0;
    virtual const SPacketStat*          GetPacketStats              ( void ) = 0;

    virtual int                         GetPing                     ( void ) = 0;
    virtual unsigned long               GetTime                     ( void ) = 0;

    virtual const char *                GetLocalIP                  ( void ) = 0;
    virtual void                        GetSerial                   ( char* szSerial, size_t maxLength ) = 0;

    virtual unsigned char               GetConnectionError          ( void ) = 0;
    virtual void                        SetConnectionError          ( unsigned char ucConnectionError ) = 0;

    virtual unsigned char               GetImmediateError           ( void ) = 0;
    virtual void                        SetImmediateError           ( unsigned char ucImmediateError ) = 0;

    virtual void                        Reset                       ( void ) = 0;

    virtual CNetHTTPDownloadManagerInterface*   GetHTTPDownloadManager          ( void ) = 0;

    virtual void                        SetServerBitStreamVersion   ( unsigned short usServerBitStreamVersion ) = 0;
    virtual unsigned short              GetServerBitStreamVersion   ( void ) = 0;

    virtual void                        GetStatus                   ( char* szStatus, size_t maxLength ) = 0;
    virtual unsigned short              GetNetRev                   ( void ) = 0;
    virtual unsigned short              GetNetRel                   ( void ) = 0;

    virtual const char*                 GetNextBuffer               ( void ) = 0;
    virtual const char*                 GetDiagnosticStatus         ( void ) = 0;

    virtual bool                        VerifySignature             ( const char* pData, unsigned long ulSize ) = 0;

    virtual void                        ResetStub                   ( DWORD dwType, ... ) = 0;
    virtual void                        ResetStub                   ( DWORD dwType, va_list ) = 0;

    virtual const char*                 GetCurrentServerId          ( void ) = 0;
    virtual bool                        CheckFile                   ( const char* szType, const char* szFilename ) = 0;

    virtual void                        SetEncryptionEnabled        ( bool bEncryptionEnabled ) = 0;
    virtual uint                        GetExtendedErrorCode        ( void ) = 0;
    virtual void                        SetTimeoutTime              ( uint uiTimeoutTime ) = 0;
};

#endif
