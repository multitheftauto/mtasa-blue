/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#ifndef MTA_CLIENT
    #include <net/ns_common.h>
#endif

typedef uint                     SSendHandle;
typedef std::shared_ptr<CBuffer> CBufferRef;

namespace LatentTransfer
{
    enum EPartFlag
    {
        FLAG_HEAD,
        FLAG_TAIL,
        FLAG_CANCEL,
    };

    enum ETransferCategory
    {
        CATEGORY_NONE,
        CATEGORY_PACKET,
    };

    const static int MIN_SEND_RATE = 500;            // Bytes per second
    const static int MIN_PACKET_SIZE = 500;
    const static int MAX_PACKET_SIZE = 1100;            // Set to 1100 as MTU is hard coded at 1200 (as of 2012-01-28)
};                                                      // namespace LatentTransfer

//
// One complete item to send
//
struct SSendItem
{
    SSendItem()
        : uiId(0),
          uiRate(0),
          usCategory(0),
          uiReadPosition(0),
          bSendStarted(false),
          bSendFinishing(false),
          pLuaMain(NULL),
          usResourceNetId(0xFFFF),
          iEstSendDurationMsRemaining(0),
          iEstSendDurationMsUsed(0)
    {
    }

    uint       uiId;                       // Handle
    CBufferRef bufferRef;                  // The data to transfer
    uint       uiRate;                     // Desired bytes per second
    ushort     usCategory;                 // Data category
    uint       uiReadPosition;             // Current position in the buffer sent so far
    bool       bSendStarted;               // true when the send actually starts
    bool       bSendFinishing;             // true when the last part has been sent
    void*      pLuaMain;                   // For cancelling by VM
    ushort     usResourceNetId;            // Only allow packet if this resource is running (ignored if 0xFFFF)

    int iEstSendDurationMsRemaining;            // Used for status calculations
    int iEstSendDurationMsUsed;                 //             ''
};

//
// One complete item to receive
//
struct SReceiveItem
{
    SReceiveItem() : usId(0), uiRate(0), usCategory(0), usResourceNetId(0xFFFF), uiWritePosition(0), bReceiveStarted(false) {}

    ushort  usId;                       // Part of handle - Used for verification
    CBuffer buffer;                     // The receive buffer
    uint    uiRate;                     // Desired bytes per second (Info from the sender - not really used for anything here yet)
    ushort  usCategory;                 // Data category
    ushort  usResourceNetId;            // Only allow packet if this resource is running (ignored if 0xFFFF)

    uint uiWritePosition;            // Current position in the buffer received so far
    bool bReceiveStarted;            // true when the receive actually starts
};

//
// Status for one queued send
//
struct SSendStatus
{
    int    iStartTimeMsOffset;            // Est. start time (Negative if already started)
    int    iEndTimeMsOffset;              // Est. end time
    int    iTotalSize;
    double dPercentComplete;            // How much done
};

///////////////////////////////////////////////////////////////
//
// CLatentSendQueue
//
// FIFO queue of SSendItems to transfer to a remote connection
//
///////////////////////////////////////////////////////////////
class CLatentSendQueue
{
public:
    ZERO_ON_NEW
    CLatentSendQueue(NetPlayerID remoteId, ushort usBitStreamVersion);
    ~CLatentSendQueue();
    void        DoPulse(int iTimeMsBetweenCalls);
    bool        OnLuaMainDestroy(void* pLuaMain);
    SSendHandle AddSend(CBufferRef bufferRef, uint uiRate, ushort usCategory, void* pLuaMain, ushort usResourceNetId);
    bool        CancelSend(SSendHandle handle);
    void        CancelAllSends();
    bool        GetSendStatus(SSendHandle handle, SSendStatus* pOutSendStatus);
    void        GetSendHandles(std::vector<SSendHandle>& outResultList);

protected:
    void SendCancelNotification(SSendItem& activeTx);
    void PostQueueRemove();
    void UpdateEstimatedDurations();

    const NetPlayerID    m_RemoteId;
    const ushort         m_usBitStreamVersion;
    std::list<SSendItem> m_TxQueue;
    uint                 m_uiCurrentRate;
    uint                 m_uiNextSendId;
    int                  m_iBytesOwing;
};

///////////////////////////////////////////////////////////////
//
// CLatentReceiver
//
// Keeps track of the current SReceiveItem being transfered from a remote connection
//
///////////////////////////////////////////////////////////////
class CLatentReceiver
{
    friend struct SScopedGuardInsideMark;

public:
    ZERO_ON_NEW
    CLatentReceiver(NetPlayerID remoteId, ushort usBitStreamVersion);
    ~CLatentReceiver();
    void OnReceive(NetBitStreamInterface* pBitStream);
    bool IsInside() const { return m_bInside; }
    bool IsDeferredDelete() const { return m_bDeferredDelete; }
    void SetDeferredDelete() { m_bDeferredDelete = true; }

protected:
    void OnReceiveError(const SString& strMessage);

    const NetPlayerID m_RemoteId;
    const ushort      m_usBitStreamVersion;
    SReceiveItem      activeRx;
    bool              m_bInside{};
    bool              m_bDeferredDelete{};
};

///////////////////////////////////////////////////////////////
//
// CLatentTransferManager
//
// Manages sendqueues and receivers for remote connections
//
///////////////////////////////////////////////////////////////
class CLatentTransferManager
{
public:
    ZERO_ON_NEW
    CLatentTransferManager();
    ~CLatentTransferManager();
    void DoPulse();
    void RemoveRemote(NetPlayerID remoteId);
    void OnLuaMainDestroy(void* pLuaMain);

    // Send functions
    void        AddSendBatchBegin(unsigned char ucPacketId, NetBitStreamInterface* pBitStream);
    SSendHandle AddSend(NetPlayerID remoteId, ushort usBitStreamVersion, uint uiRate, void* pLuaMain, ushort usResourceNetId);
    void        AddSendBatchEnd();

    bool CancelSend(NetPlayerID remoteId, SSendHandle handle);
    void CancelAllSends(NetPlayerID remoteId);
    bool GetSendStatus(NetPlayerID remoteId, SSendHandle handle, SSendStatus* pOutSendStatus);
    void GetSendHandles(NetPlayerID remoteId, std::vector<SSendHandle>& outResultList);

    // Receive functions
    void OnReceive(NetPlayerID remoteId, NetBitStreamInterface* pBitStream);

protected:
    CLatentSendQueue* GetSendQueueForRemote(NetPlayerID remoteId, ushort usBitStreamVersion);
    CLatentSendQueue* FindSendQueueForRemote(NetPlayerID remoteId);
    CLatentReceiver*  GetReceiverForRemote(NetPlayerID remoteId, ushort usBitStreamVersion);
    CLatentReceiver*  FindReceiverForRemote(NetPlayerID remoteId);

    CTickCount m_LastTimeMs;
    int        m_iTimeMsBetweenCalls;
    uint       m_uiNumSends;

    // Send variables
    std::vector<CLatentSendQueue*>           m_SendQueueList;
    std::map<NetPlayerID, CLatentSendQueue*> m_SendQueueMap;
    CBufferRef                               m_pBatchBufferRef;

    // Receive variables
    std::map<NetPlayerID, CLatentReceiver*> m_ReceiverMap;
};

//
// Some net function helpers that need different implementations on client and server
//
NetBitStreamInterface* DoAllocateNetBitStream(NetPlayerID remoteId, ushort usBitStreamVersion);
void                   DoDeallocateNetBitStream(NetBitStreamInterface* pBitStream);
bool                   DoSendPacket(unsigned char ucPacketID, NetPlayerID remoteId, NetBitStreamInterface* bitStream, NetPacketPriority packetPriority,
                                    NetPacketReliability packetReliability, ePacketOrdering packetOrdering = PACKET_ORDERING_DEFAULT);
bool                   DoStaticProcessPacket(unsigned char ucPacketID, NetPlayerID remoteId, NetBitStreamInterface* pBitStream, ushort usResourceNetId);
void                   DoDisconnectRemote(NetPlayerID remoteId, const SString& strReason);
