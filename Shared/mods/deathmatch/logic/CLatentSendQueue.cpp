/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLatentTransferManager.h"
#include <net/Packets.h>

using namespace LatentTransfer;

///////////////////////////////////////////////////////////////
//
// CLatentSendQueue::CLatentSendQueue
//
//
//
///////////////////////////////////////////////////////////////
CLatentSendQueue::CLatentSendQueue(NetPlayerID remoteId, ushort usBitStreamVersion) : m_RemoteId(remoteId), m_usBitStreamVersion(usBitStreamVersion)
{
}

///////////////////////////////////////////////////////////////
//
// CLatentSendQueue::~CLatentSendQueue
//
//
//
///////////////////////////////////////////////////////////////
CLatentSendQueue::~CLatentSendQueue()
{
}

///////////////////////////////////////////////////////////////
//
// CLatentSendQueue::DoPulse
//
// Send next part of the active transfer
//
///////////////////////////////////////////////////////////////
void CLatentSendQueue::DoPulse(int iTimeMsBetweenCalls)
{
    if (m_TxQueue.empty())
    {
        m_iBytesOwing = 0;
        return;
    }

    // Check if previous tx has completed
    if (m_TxQueue.front().uiReadPosition == m_TxQueue.front().bufferRef->GetSize() && m_TxQueue.front().bSendFinishing)
    {
        m_TxQueue.pop_front();
        PostQueueRemove();
        if (m_TxQueue.empty())
        {
            m_iBytesOwing = 0;
            return;
        }
    }

    m_uiCurrentRate = std::max<uint>(MIN_SEND_RATE, m_uiCurrentRate);

    // How many bytes to send this pulse
    int iBytesToSendThisPulse = iTimeMsBetweenCalls * m_uiCurrentRate / 1000;

    // Add bytes owing from last pulse
    iBytesToSendThisPulse += m_iBytesOwing;

    // Calc packet size depending on rate
    uint uiMaxPacketSize = Lerp(MIN_PACKET_SIZE, UnlerpClamped(MIN_PACKET_SIZE * 10, m_uiCurrentRate, MAX_PACKET_SIZE * 15), MAX_PACKET_SIZE);

    // Calc how many packets to do this pulse
    uint uiNumPackets = iBytesToSendThisPulse / uiMaxPacketSize;

    // Update carry over
    m_iBytesOwing = iBytesToSendThisPulse % uiMaxPacketSize;

    // Process item at front of queue
    SSendItem& activeTx = m_TxQueue.front();
    for (uint i = 0; i < uiNumPackets && !activeTx.bSendFinishing; i++)
    {
        // Send next part of data
        NetBitStreamInterface* pBitStream = DoAllocateNetBitStream(m_RemoteId, m_usBitStreamVersion);
        pBitStream->WriteBits(&activeTx.uiId, 15);

        // Next bit indicates if it has a special flag
        if (activeTx.uiReadPosition == 0)
        {
            // Head
            pBitStream->WriteBit(1);
            pBitStream->Write((uchar)FLAG_HEAD);
            pBitStream->Write(activeTx.usCategory);
            pBitStream->Write(activeTx.bufferRef->GetSize());
            pBitStream->Write(activeTx.uiRate);
            if (pBitStream->Version() >= 0x31)
                pBitStream->Write(activeTx.usResourceNetId);
            activeTx.bSendStarted = true;
        }
        else if (activeTx.bufferRef->GetSize() == activeTx.uiReadPosition)
        {
            // Tail
            pBitStream->WriteBit(1);
            pBitStream->Write((uchar)FLAG_TAIL);
            activeTx.bSendFinishing = true;
        }
        else
        {
            // Body
            pBitStream->WriteBit(0);
        }

        // Align to next boundary
        pBitStream->AlignWriteToByteBoundary();
        uint uiMaxDataSize = std::max<int>(10, uiMaxPacketSize - pBitStream->GetNumberOfBytesUsed());

        // Calc how much data to send
        uint uiDataOffset = activeTx.uiReadPosition;
        uint uiSizeToSend = std::min(uiMaxDataSize, activeTx.bufferRef->GetSize() - activeTx.uiReadPosition);
        activeTx.uiReadPosition += uiSizeToSend;

        pBitStream->Write((ushort)uiSizeToSend);
        pBitStream->Write(activeTx.bufferRef->GetData() + uiDataOffset, uiSizeToSend);

        // Send
        DoSendPacket(PACKET_ID_LATENT_TRANSFER, m_RemoteId, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_RELIABLE_ORDERED,
                     PACKET_ORDERING_DATA_TRANSFER);
        DoDeallocateNetBitStream(pBitStream);
    }
}

///////////////////////////////////////////////////////////////
//
// CLatentSendQueue::AddSend
//
// From data pointer
//
///////////////////////////////////////////////////////////////
SSendHandle CLatentSendQueue::AddSend(CBufferRef bufferRef, uint uiRate, ushort usCategory, void* pLuaMain, ushort usResourceNetId)
{
    SSendItem& newTx = m_TxQueue.emplace_back();

    newTx.uiId = m_uiNextSendId++;
    newTx.bufferRef = std::move(bufferRef);
    newTx.uiRate = uiRate;
    newTx.usCategory = usCategory;
    newTx.pLuaMain = pLuaMain;
    newTx.usResourceNetId = usResourceNetId;

    // Current rate is highest queued item
    m_uiCurrentRate = std::max(m_uiCurrentRate, newTx.uiRate);

    return newTx.uiId;
}

///////////////////////////////////////////////////////////////
//
// CLatentSendQueue::CancelSend
//
// Returns true if transfer was cancelled.
//
///////////////////////////////////////////////////////////////
bool CLatentSendQueue::CancelSend(SSendHandle handle)
{
    for (std::list<SSendItem>::iterator iter = m_TxQueue.begin(); iter != m_TxQueue.end(); ++iter)
    {
        if (iter->uiId == handle)
        {
            if (iter->bSendStarted)
            {
                if (iter->bSendFinishing)
                {
                    // Too late to cancel
                    return false;
                }

                SendCancelNotification(*iter);
            }

            m_TxQueue.erase(iter);
            PostQueueRemove();
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////
//
// CLatentSendQueue::CancelAllSends
//
//
//
///////////////////////////////////////////////////////////////
void CLatentSendQueue::CancelAllSends()
{
    // Use a copy because CancelSend will not remove a finishing transfer
    std::list<SSendItem> listCopy = m_TxQueue;
    for (std::list<SSendItem>::iterator iter = listCopy.begin(); iter != listCopy.end(); ++iter)
        CancelSend(iter->uiId);
    m_TxQueue.clear();
}

///////////////////////////////////////////////////////////////
//
// CLatentSendQueue::GetSendStatus
//
// Return start and end ticks for the send
//
///////////////////////////////////////////////////////////////
bool CLatentSendQueue::GetSendStatus(SSendHandle handle, SSendStatus* pOutSendStatus)
{
    UpdateEstimatedDurations();

    int iTimeMsBefore = 0;
    for (std::list<SSendItem>::iterator iter = m_TxQueue.begin(); iter != m_TxQueue.end(); ++iter)
    {
        if (iter->uiId == handle)
        {
            pOutSendStatus->iStartTimeMsOffset = iTimeMsBefore - iter->iEstSendDurationMsUsed;
            pOutSendStatus->iEndTimeMsOffset = iTimeMsBefore + iter->iEstSendDurationMsRemaining;
            pOutSendStatus->iTotalSize = iter->bufferRef->GetSize();
            pOutSendStatus->dPercentComplete = iter->uiReadPosition * 100.0 / std::max(1, pOutSendStatus->iTotalSize);
            return true;
        }
        iTimeMsBefore += iter->iEstSendDurationMsRemaining;
    }

    return false;
}

///////////////////////////////////////////////////////////////
//
// CLatentSendQueue::GetSendHandles
//
// Get all queued send handles
//
///////////////////////////////////////////////////////////////
void CLatentSendQueue::GetSendHandles(std::vector<SSendHandle>& outResultList)
{
    outResultList.clear();
    for (std::list<SSendItem>::iterator iter = m_TxQueue.begin(); iter != m_TxQueue.end(); ++iter)
    {
        outResultList.push_back(iter->uiId);
    }
}

///////////////////////////////////////////////////////////////
//
// CLatentSendQueue::UpdateEstimatedDurations
//
//
//
///////////////////////////////////////////////////////////////
void CLatentSendQueue::UpdateEstimatedDurations()
{
    uint uiUsingRate = MIN_SEND_RATE;
    // Recalculate estimated times for all transfers
    for (std::list<SSendItem>::reverse_iterator iter = m_TxQueue.rbegin(); iter != m_TxQueue.rend(); ++iter)
    {
        SSendItem& tx = *iter;

        uiUsingRate = std::max(uiUsingRate, tx.uiRate);
        tx.iEstSendDurationMsRemaining = tx.bufferRef->GetSize() * 1000 / uiUsingRate;
        tx.iEstSendDurationMsUsed = 0;

        if (tx.bSendStarted)
        {
            // Calc how long to go for a send that is in-progress
            int iSizeRemaining = tx.bufferRef->GetSize() - tx.uiReadPosition;
            int iTimeMsRemaining = iSizeRemaining * 1000 / m_uiCurrentRate;
            tx.iEstSendDurationMsUsed = tx.iEstSendDurationMsRemaining - iTimeMsRemaining;
            tx.iEstSendDurationMsRemaining = iTimeMsRemaining;
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CLatentSendQueue::PostQueueRemove
//
// Called when a transfer is removed from the queue.
//
///////////////////////////////////////////////////////////////
void CLatentSendQueue::PostQueueRemove()
{
    // Recalculate the current transfer rate
    m_uiCurrentRate = MIN_SEND_RATE;
    for (std::list<SSendItem>::iterator iter = m_TxQueue.begin(); iter != m_TxQueue.end(); ++iter)
        m_uiCurrentRate = std::max(m_uiCurrentRate, iter->uiRate);
}

///////////////////////////////////////////////////////////////
//
// CLatentSendQueue::SendCancelNotification
//
// Tell remote an in-progress transfer is cancelled
//
///////////////////////////////////////////////////////////////
void CLatentSendQueue::SendCancelNotification(SSendItem& activeTx)
{
    assert(activeTx.bSendStarted && !activeTx.bSendFinishing);
    NetBitStreamInterface* pBitStream = DoAllocateNetBitStream(m_RemoteId, m_usBitStreamVersion);
    pBitStream->WriteBits(&activeTx.uiId, 15);
    pBitStream->WriteBit(1);
    pBitStream->Write((uchar)FLAG_CANCEL);
    DoSendPacket(PACKET_ID_LATENT_TRANSFER, m_RemoteId, pBitStream, PACKET_PRIORITY_LOW, PACKET_RELIABILITY_RELIABLE_ORDERED, PACKET_ORDERING_DATA_TRANSFER);
    DoDeallocateNetBitStream(pBitStream);
}

///////////////////////////////////////////////////////////////
//
// CLatentSendQueue::OnLuaMainDestroy
//
// When a Lua VM is stopped
//
///////////////////////////////////////////////////////////////
bool CLatentSendQueue::OnLuaMainDestroy(void* pLuaMain)
{
    for (std::list<SSendItem>::iterator iter = m_TxQueue.begin(); iter != m_TxQueue.end();)
    {
        if (iter->pLuaMain == pLuaMain && !iter->bSendFinishing)
        {
            if (iter->bSendStarted)
                SendCancelNotification(*iter);

            iter = m_TxQueue.erase(iter);
            PostQueueRemove();
        }
        else
            ++iter;
    }
    return false;
}
