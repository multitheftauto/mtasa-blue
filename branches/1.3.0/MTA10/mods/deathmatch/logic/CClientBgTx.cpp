/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "CClientBgTx.h"

#define PACKET_ID_BG_TX 123
#define BG_TX_HEAD 1
#define BG_TX_TAIL 2
#define BG_TX_CANCEL 3

///////////////////////////////////////////////////////////////
//
// CClientBgTx::CClientBgTx
//
//
//
///////////////////////////////////////////////////////////////
CClientBgTx::CClientBgTx ( void )
    : m_uiMaxPacketSize ( 500 )
{
}


///////////////////////////////////////////////////////////////
//
// CClientBgTx::~CClientBgTx
//
//
//
///////////////////////////////////////////////////////////////
CClientBgTx::~CClientBgTx ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CClientBgTx::DoPulse
//
// Send next packet when it is time
//
///////////////////////////////////////////////////////////////
void CClientBgTx::DoPulse ( void )
{
    if ( m_TxQueue.empty () || CTickCount::Now () < m_NextSentTime )
        return;

    // Check if previous tx has completed
    if ( m_TxQueue.front ().uiReadPosition == m_TxQueue.front ().buffer.GetSize () )
    {
        m_TxQueue.pop_front ();
        PostQueueRemove ();
    }

    SClientTx& activeTx = m_TxQueue.front ();


    // Send next part of data    
    NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();

    // Transfer id
    pBitStream->WriteBits ( &activeTx.uiId, 15 );

    // Next bit indicates if it is a special packet
    if ( activeTx.uiReadPosition == 0 )
    {
        // First
        pBitStream->WriteBit ( 1 );
        pBitStream->Write ( (uchar)BG_TX_HEAD );
        pBitStream->Write ( activeTx.usCategory );
        pBitStream->Write ( activeTx.buffer.GetSize () );
        pBitStream->Write ( activeTx.uiRate );
        activeTx.bTransferStarted = true;
    }
    else
    if ( activeTx.buffer.GetSize () == activeTx.uiReadPosition )
    {
        // Last
        pBitStream->WriteBit ( 1 );
        pBitStream->Write ( (uchar)BG_TX_TAIL );
        activeTx.bTransferFinishing = true;
    }
    else
    {
        // Body
        pBitStream->WriteBit ( 0 );
    }

    // Align to next boundary
    pBitStream->AlignWriteToByteBoundary ();
    uint uiMaxDataSize = Max < int > ( 10, m_uiMaxPacketSize - pBitStream->GetNumberOfBytesUsed () );

    // Calc how much data to send
    uint uiDataOffset = activeTx.uiReadPosition;
    uint uiSizeToSend = Min ( uiMaxDataSize, activeTx.buffer.GetSize () - activeTx.uiReadPosition );
    activeTx.uiReadPosition += uiSizeToSend;

    pBitStream->Write ( (ushort)uiSizeToSend );
    pBitStream->Write ( activeTx.buffer.GetData () + uiDataOffset, uiSizeToSend );

    g_pNet->SendPacket ( PACKET_ID_BG_TX, pBitStream, PACKET_PRIORITY_MEDIUM, PACKET_RELIABILITY_RELIABLE_ORDERED, PACKET_ORDERING_CHAT );
    g_pNet->DeallocateNetBitStream ( pBitStream );

    //                          1000 / 5000
    //                          1000 / 500
    long long llTicksUntilNextSend = uiSizeToSend * 1000 / Max ( 500U, m_uiCurrentRate );
    m_NextSentTime = CTickCount::Now () + CTickCount ( llTicksUntilNextSend );
}


///////////////////////////////////////////////////////////////
//
// CClientBgTx::AddTransfer
//
//
//
///////////////////////////////////////////////////////////////
STransferHandle CClientBgTx::AddTransfer ( uchar ucPacketId, NetBitStreamInterface* pBitStream, uint uiRate, ushort usCategory )
{
    SClientTx newTx;
    newTx.uiId = m_uiNextTransferId++;
    //newTx.pBitStream = pBitStream;
    newTx.ucPacketId = ucPacketId;
    newTx.uiRate = uiRate;
    newTx.usCategory = usCategory;
    newTx.uiReadPosition = 0;
    newTx.bTransferStarted = false;
    newTx.bTransferFinishing = false;

    //newTx.pBitStream->AddRef ();

    // Copy data from bitstream into buffer
    uint uiBitsUsed = pBitStream->GetNumberOfBitsUsed ();
    uint uiBytesUsed = pBitStream->GetNumberOfBytesUsed ();
    newTx.buffer.SetSize ( uiBytesUsed );
    pBitStream->ResetReadPointer ();
    pBitStream->Read ( newTx.buffer.GetData (), newTx.buffer.GetSize () );

    m_TxQueue.push_back ( newTx );

    // Current rate is highest queued item
    m_uiCurrentRate = Max ( m_uiCurrentRate, newTx.uiRate );

    return newTx.uiId;
}


///////////////////////////////////////////////////////////////
//
// CClientBgTx::PostQueueRemove
//
// Called when a transfer is removed from the queue.
//
///////////////////////////////////////////////////////////////
void CClientBgTx::PostQueueRemove ( void )
{
    // Recalculate the current transfer rate
    m_uiCurrentRate = 500;
    for ( std::list < SClientTx >::iterator iter = m_TxQueue.begin () ; iter != m_TxQueue.end () ; ++iter )
        m_uiCurrentRate = Max ( m_uiCurrentRate, iter->uiRate );
}


///////////////////////////////////////////////////////////////
//
// CClientBgTx::CancelTransfer
//
// Returns true if transfer was cancelled.
//
///////////////////////////////////////////////////////////////
bool CClientBgTx::CancelTransfer ( STransferHandle handle )
{
    for ( std::list < SClientTx >::iterator iter = m_TxQueue.begin () ; iter != m_TxQueue.end () ; ++iter )
    {
        if ( iter->uiId == handle )
        {
            if ( iter->bTransferStarted )
            {
                if ( iter->bTransferFinishing )
                {
                    // Too late to cancel
                    return false;
                }

                // Send cancel notification
                SClientTx& activeTx = *iter;
                NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream ();
                pBitStream->WriteBits ( &activeTx.uiId, 15 );
                pBitStream->WriteBit ( 1 );
                pBitStream->Write ( (uchar)BG_TX_CANCEL );
                g_pNet->SendPacket ( PACKET_ID_BG_TX, pBitStream, PACKET_PRIORITY_MEDIUM, PACKET_RELIABILITY_RELIABLE_ORDERED, PACKET_ORDERING_CHAT );
                g_pNet->DeallocateNetBitStream ( pBitStream );
            }

            m_TxQueue.erase ( iter );
            PostQueueRemove ();
            return true;
        }
    }
    return false;
}


///////////////////////////////////////////////////////////////
//
// CClientBgTx::CancelAllTransfers
//
//
//
///////////////////////////////////////////////////////////////
void CClientBgTx::CancelAllTransfers ( void )
{
    // Use a copy because CancelTransfer will not remove a finishing transfer
    std::list < SClientTx > listCopy = m_TxQueue;
    for ( std::list < SClientTx >::iterator iter = listCopy.begin () ; iter != listCopy.end () ; ++iter )
        CancelTransfer ( iter->uiId );
    m_TxQueue.clear ();
}


///////////////////////////////////////////////////////////////
//
// CClientBgTx::GetTransferStatus
//
//
//
///////////////////////////////////////////////////////////////
bool CClientBgTx::GetTransferStatus ( STransferHandle handle, uint* puiTickCountStart, uint* puiTickCountEnd )
{
    // Recalculate estimated times for all tranfers upto the one requested
    for ( std::list < SClientTx >::iterator iter = m_TxQueue.begin () ; iter != m_TxQueue.end () ; ++iter )
    {
        

        if ( iter->uiId == handle )
        {
            
            return true;
        }
    }
    return false;
}


///////////////////////////////////////////////////////////////
//
// CClientBgTx::GetlAllTransferStatus
//
//
//
///////////////////////////////////////////////////////////////
uint CClientBgTx::GetlAllTransferStatus ( void )
{
    return 0;
}


///////////////////////////////////////////////////////////////
//
// CClientBgTx::Pause
//
//
//
///////////////////////////////////////////////////////////////
void CClientBgTx::Pause ( bool bPause )
{
}
