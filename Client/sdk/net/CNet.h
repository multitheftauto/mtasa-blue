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

#pragma once

#include <string>
#include "net_common.h"
#include "net/bitstream.h"
#include "net/CNetHTTPDownloadManagerInterface.h"

#define MAX_CALL_REMOTE_QUEUES  100

namespace EDownloadMode
{
    enum EDownloadModeType
    {
        NONE,
        CORE_ASE_LIST,
        CORE_UPDATER,
        WEBBROWSER_LISTS,
        CORE_LAST,            // Download modes after this one will be reset on server disconnect
        RESOURCE_INITIAL_FILES_INTERNAL,
        RESOURCE_INITIAL_FILES_EXTERNAL,
        RESOURCE_SINGULAR_FILES,
        CONNECT_TCP_SEND,
        CALL_REMOTE_RESTRICTED,
        CALL_REMOTE_RESTRICTED_LAST = CALL_REMOTE_RESTRICTED + MAX_CALL_REMOTE_QUEUES - 1,
        CALL_REMOTE_ANY_HOST,
        CALL_REMOTE_ANY_HOST_LAST = CALL_REMOTE_ANY_HOST + MAX_CALL_REMOTE_QUEUES - 1,
    };
}
using EDownloadMode::EDownloadModeType;

struct SPacketStat
{
    int    iCount;
    int    iTotalBytes;
    TIMEUS totalTime;
};

class CBinaryFileInterface
{
public:
    virtual ~CBinaryFileInterface() {}
    virtual bool FOpen(const char* szFilename, const char* szMode, bool bValidate) = 0;
    virtual void FClose() = 0;
    virtual bool FEof() = 0;
    virtual void FFlush() = 0;
    virtual int  FTell() = 0;
    virtual void FSeek(int iOffset, int iOrigin) = 0;
    virtual int  FRead(void* pData, uint uiSize) = 0;
    virtual int  FWrite(const void* pData, uint uiSize) = 0;
};

class CNet
{
public:
    enum ENetworkUsageDirection
    {
        STATS_INCOMING_TRAFFIC = 0,
        STATS_OUTGOING_TRAFFIC = 1
    };

    virtual bool StartNetwork(const char* szServerHost, unsigned short usServerPort, bool bPacketTag = false) = 0;
    virtual void StopNetwork() = 0;

    virtual void SetFakeLag(unsigned short usPacketLoss, unsigned short usMinExtraPing, unsigned short usExtraPingVariance, int iKBPSLimit) = 0;

    virtual bool IsConnected() = 0;

    virtual void DoPulse() = 0;
    virtual void Shutdown() = 0;

    virtual void RegisterPacketHandler(PPACKETHANDLER pfnPacketHandler) = 0;

    virtual NetBitStreamInterface* AllocateNetBitStream() = 0;
    virtual void                   DeallocateNetBitStream(NetBitStreamInterface* bitStream) = 0;
    virtual bool                   SendPacket(unsigned char ucPacketID, NetBitStreamInterface* bitStream, NetPacketPriority packetPriority,
                                              NetPacketReliability packetReliability, ePacketOrdering packetOrdering = PACKET_ORDERING_DEFAULT) = 0;

    virtual void        SetClientPort(unsigned short usClientPort) = 0;
    virtual const char* GetConnectedServer(bool bIncludePort = false) = 0;

    virtual bool               GetNetworkStatistics(NetStatistics* pDest) = 0;
    virtual const SPacketStat* GetPacketStats() = 0;

    virtual int           GetPing() = 0;
    virtual unsigned long GetTime() = 0;

    virtual const char* GetLocalIP() = 0;
    virtual void        GetSerial(char* szSerial, size_t maxLength) = 0;

    virtual unsigned char GetConnectionError() = 0;
    virtual void          SetConnectionError(unsigned char ucConnectionError) = 0;

    virtual void Reset() = 0;

    virtual CNetHTTPDownloadManagerInterface* GetHTTPDownloadManager(EDownloadModeType iMode) = 0;

    virtual void           SetServerBitStreamVersion(unsigned short usServerBitStreamVersion) = 0;
    virtual unsigned short GetServerBitStreamVersion() = 0;
    bool                   CanServerBitStream(eBitStreamVersion query) { return static_cast<eBitStreamVersion>(GetServerBitStreamVersion()) >= query; }

    virtual void           GetStatus(char* szStatus, size_t maxLength) = 0;
    virtual unsigned short GetNetRev() = 0;
    virtual unsigned short GetNetRel() = 0;

    virtual const char* GetNextBuffer() = 0;
    virtual const char* GetDiagnosticStatus() = 0;
    virtual void        UpdatePingStatus(const char* status, size_t statusLength, ushort& usDataRef, bool& isVerified) = 0;

    virtual bool VerifySignature(const char* pData, unsigned long ulSize) = 0;

    virtual void ResetStub(DWORD dwType, ...) = 0;
    virtual void ResetStub(DWORD dwType, va_list) = 0;

    virtual const char* GetCurrentServerId(bool bPreviousVer) = 0;
    virtual bool        CheckFile(const char* szType, const char* szFilename, const void* pData = nullptr, size_t sizeData = 0) = 0;

    virtual uint GetExtendedErrorCode() = 0;
    virtual void SetTimeoutTime(uint uiTimeoutTime) = 0;

    virtual bool                  ValidateBinaryFileName(const char* szFilename) = 0;
    virtual CBinaryFileInterface* AllocateBinaryFile() = 0;
    virtual bool                  EncryptDumpfile(const char* szClearPathFilename, const char* szEncryptedPathFilename) = 0;
    virtual bool DeobfuscateScript(const char* cpInBuffer, uint uiInSize, const char** pcpOutBuffer, uint* puiOutSize, const char* szScriptName) = 0;
    virtual void PostCrash() = 0;
    virtual int  SendTo(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen) = 0;
};
