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

namespace EDownloadMode
{
    enum EDownloadModeType
    {
        NONE,
        CALL_REMOTE,
        ASE,
    };
}
using EDownloadMode::EDownloadModeType;


struct SPacketStat
{
    int iCount;
    int iTotalBytes;
    TIMEUS totalTime;
};

class CBinaryFileInterface
{
public:
    virtual             ~CBinaryFileInterface   ( void ) {}
    virtual bool        FOpen                   ( const char* szFilename, const char* szMode, bool bValidate ) = 0;
    virtual void        FClose                  ( void ) = 0;
    virtual bool        FEof                    ( void ) = 0;
    virtual void        FFlush                  ( void ) = 0;
    virtual int         FTell                   ( void ) = 0;
    virtual void        FSeek                   ( int iOffset, int iOrigin ) = 0;
    virtual int         FRead                   ( void* pData, uint uiSize ) = 0;
    virtual int         FWrite                  ( const void* pData, uint uiSize ) = 0;
};

struct SNetOptions
{
    SNetOptions ( void ) { memset ( this, 0, sizeof( *this ) ); }

    struct {
        bool bValid;
        int iPacketLoss;
        int iExtraPing;
        int iExtraPingVariance;
        int iKBPSLimit;
    } netSim;

    struct {
        bool bValid;
        bool bAutoFilter;
    } netFilter;

    struct {
        bool bValid;
        int iUpdateCycleDatagramsLimit;
        int iUpdateCycleMessagesLimit;
    } netOptimize;
};

struct SScriptInfo
{
    const char* szMinServerHostVer;
    const char* szMinServerRunVer;
    const char* szMinClientRunVer;
};

struct SPlayerPacketUsage
{
    NetServerPlayerID playerId;
    uint uiPktsPerSec;
    uint uiBytesPerSec;
};

class CNetServer
{
public:
    enum ENetworkUsageDirection
    {
        STATS_INCOMING_TRAFFIC = 0,
        STATS_OUTGOING_TRAFFIC = 1
    };

    // szIP can be NULL if autochoosing is wanted.
    virtual bool                            StartNetwork                    ( const char* szIP, unsigned short usServerPort, unsigned int uiAllowedPlayers, const char* szServerName ) = 0;
    virtual void                            StopNetwork                     ( void ) = 0;

    virtual void                            DoPulse                         ( void ) = 0;

    virtual void                            RegisterPacketHandler           ( PPACKETHANDLER pfnPacketHandler ) = 0;

    virtual bool                            GetNetworkStatistics            ( NetStatistics* pDest, const NetServerPlayerID& PlayerID ) = 0;
    virtual const SPacketStat*              GetPacketStats                  ( void ) = 0;
    virtual bool                            GetBandwidthStatistics          ( SBandwidthStatistics* pDest ) = 0;
    virtual bool                            GetNetPerformanceStatistics     ( SNetPerformanceStatistics* pDest, bool bResetCounters ) = 0;
    virtual void                            GetPingStatus                   ( SFixedString < 32 >* pstrStatus ) = 0;
    virtual bool                            GetSyncThreadStatistics         ( SSyncThreadStatistics* pDest, bool bResetCounters ) = 0;

    virtual NetBitStreamInterface*          AllocateNetServerBitStream      ( unsigned short usBitStreamVersion, const void* pData = nullptr, uint uiDataSize = 0, bool bCopyData = false ) = 0;
    virtual void                            DeallocateNetServerBitStream    ( NetBitStreamInterface* bitStream ) = 0;
    virtual bool                            SendPacket                      ( unsigned char ucPacketID, const NetServerPlayerID& playerID, NetBitStreamInterface* bitStream, bool bBroadcast, NetServerPacketPriority packetPriority, NetServerPacketReliability packetReliability, ePacketOrdering packetOrdering = PACKET_ORDERING_DEFAULT ) = 0;

    virtual void                            GetPlayerIP                     ( const NetServerPlayerID& playerID, char strIP[22], unsigned short* usPort ) = 0;

    virtual void                            Kick                            ( const NetServerPlayerID &PlayerID ) = 0;

    virtual void                            SetPassword                     ( const char* szPassword ) = 0;

    virtual void                            SetMaximumIncomingConnections   ( unsigned short numberAllowed ) = 0;

    virtual CNetHTTPDownloadManagerInterface*   GetHTTPDownloadManager      ( EDownloadModeType iMode ) = 0;

    virtual void                            SetClientBitStreamVersion       ( const NetServerPlayerID &PlayerID, unsigned short usBitStreamVersion ) = 0;
    virtual void                            ClearClientBitStreamVersion     ( const NetServerPlayerID &PlayerID ) = 0;

    virtual void                            SetChecks                       ( const char* szDisableComboACMap, const char* szDisableACMap, const char* szEnableSDMap, int iEnableClientChecks, bool bHideAC, const char* szImgMods ) = 0;

    virtual unsigned int                    GetPendingPacketCount           ( void ) = 0;
    virtual void                            GetNetRoute                     ( SFixedString < 32 >* pstrRoute ) = 0;

    virtual bool                            InitServerId                    ( const char* szPath ) = 0;
    virtual void                            ResendModPackets                ( const NetServerPlayerID& playerID ) = 0;
    virtual void                            ResendACPackets                 ( const NetServerPlayerID& playerID ) = 0;

    virtual void                            GetClientSerialAndVersion       ( const NetServerPlayerID& playerID, SFixedString < 32 >& strSerial, SFixedString < 64 >& strExtra, SFixedString < 32 >& strVersion ) = 0;
    virtual void                            SetNetOptions                   ( const SNetOptions& options ) = 0;
    virtual void                            GenerateRandomData              ( void* pOutData, uint uiLength ) = 0;
    virtual bool                            EncryptDumpfile                 ( const char* szClearPathFilename, const char* szEncryptedPathFilename ) { return false; }
    virtual bool                            ValidateHttpCacheFileName       ( const char* szFilename ) { return false; }
    virtual bool                            GetScriptInfo                   ( const char* cpInBuffer, uint uiInSize, SScriptInfo* pOutInfo ) { return false; }
    virtual bool                            DeobfuscateScript               ( const char* cpInBuffer, uint uiInSize, const char** pcpOutBuffer, uint* puiOutSize, const char* szScriptName ) { return false; }
    virtual bool                            GetPlayerPacketUsageStats       ( uchar* packetIdList, uint uiNumPacketIds, SPlayerPacketUsage* pOutStats, uint uiTopCount ) { return false; }
    virtual const char*                     GetLogOutput                    ( void )                                    { return NULL; }
    virtual bool                            IsValidSocket                   ( const NetServerPlayerID& playerID )       { assert( 0 ); return false; }
};

#endif

