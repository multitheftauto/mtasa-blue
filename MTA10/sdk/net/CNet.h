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

namespace EDownloadMode
{
    enum EDownloadModeType
    {
        NONE,
        CORE_ASE_LIST,
        CORE_UPDATER,
        RESOURCE_INITIAL_FILES,
        RESOURCE_SINGULAR_FILES,
        CALL_REMOTE,
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

    virtual void                        SetFakeLag                  ( unsigned short usPacketLoss, unsigned short usMinExtraPing, unsigned short usExtraPingVariance, int iKBPSLimit ) = 0;

    virtual bool                        IsConnected                 ( void ) = 0;

    virtual void                        DoPulse                     ( void ) = 0;
    virtual void                        Shutdown                    ( void ) = 0;

    virtual void                        RegisterPacketHandler       ( PPACKETHANDLER pfnPacketHandler ) = 0;

    virtual NetBitStreamInterface*      AllocateNetBitStream        ( void ) = 0;
    virtual void                        DeallocateNetBitStream      ( NetBitStreamInterface* bitStream ) = 0;
    virtual bool                        SendPacket                  ( unsigned char ucPacketID, NetBitStreamInterface* bitStream, NetPacketPriority packetPriority, NetPacketReliability packetReliability, ePacketOrdering packetOrdering = PACKET_ORDERING_DEFAULT ) = 0;

    virtual void                        SetClientPort               ( unsigned short usClientPort ) = 0;
    virtual const char *                GetConnectedServer          ( bool bIncludePort = false )=0;

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

    virtual CNetHTTPDownloadManagerInterface*   GetHTTPDownloadManager          ( EDownloadModeType iMode ) = 0;

    virtual void                        SetServerBitStreamVersion   ( unsigned short usServerBitStreamVersion ) = 0;
    virtual unsigned short              GetServerBitStreamVersion   ( void ) = 0;

    virtual void                        GetStatus                   ( char* szStatus, size_t maxLength ) = 0;
    virtual unsigned short              GetNetRev                   ( void ) = 0;
    virtual unsigned short              GetNetRel                   ( void ) = 0;

    virtual const char*                 GetNextBuffer               ( void ) = 0;
    virtual const char*                 GetDiagnosticStatus         ( void ) = 0;
    virtual void                        UpdatePingStatus            ( const char* szStatus, ushort& usDataRef ) = 0;

    virtual bool                        VerifySignature             ( const char* pData, unsigned long ulSize ) = 0;

    virtual void                        ResetStub                   ( DWORD dwType, ... ) = 0;
    virtual void                        ResetStub                   ( DWORD dwType, va_list ) = 0;

    virtual const char*                 GetCurrentServerId          ( bool bPreviousVer ) = 0;
    virtual bool                        CheckFile                   ( const char* szType, const char* szFilename ) = 0;

    virtual uint                        GetExtendedErrorCode        ( void ) = 0;
    virtual void                        SetTimeoutTime              ( uint uiTimeoutTime ) = 0;

    virtual bool                        ValidateBinaryFileName      ( const char* szFilename ) = 0;
    virtual CBinaryFileInterface*       AllocateBinaryFile          ( void ) = 0;
    virtual bool                        EncryptDumpfile             ( const char* szClearPathFilename, const char* szEncryptedPathFilename ) = 0;
    virtual bool                        DecryptScript               ( const char* cpInBuffer, uint uiInSize, const char** pcpOutBuffer, uint* puiOutSize, const char* szScriptName ) = 0;
};

#endif
