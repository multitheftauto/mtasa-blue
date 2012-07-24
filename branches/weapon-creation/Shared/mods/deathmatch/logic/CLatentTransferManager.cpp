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
using namespace LatentTransfer;


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::CLatentTransferManager
//
//
//
///////////////////////////////////////////////////////////////
CLatentTransferManager::CLatentTransferManager ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::~CLatentTransferManager
//
//
//
///////////////////////////////////////////////////////////////
CLatentTransferManager::~CLatentTransferManager ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::DoPulse
//
//
//
///////////////////////////////////////////////////////////////
void CLatentTransferManager::DoPulse ( void )
{
    // Update timing info
    CTickCount tickCountNow = CTickCount::Now ();

    int iDeltaTimeMs = (int)( tickCountNow - m_LastTimeMs ).ToLongLong ();
    m_LastTimeMs = tickCountNow;

    // Smooth out time between calls, with more resistance when rising
    int iBetweenCallsChange = iDeltaTimeMs - m_iTimeMsBetweenCalls;
    if ( iBetweenCallsChange > 0 )
        m_iTimeMsBetweenCalls += Min ( iBetweenCallsChange, Max ( 1, m_iTimeMsBetweenCalls / 10 ) );   // 10% max when rising
    else
        m_iTimeMsBetweenCalls -= Min ( -iBetweenCallsChange, Max ( 1, m_iTimeMsBetweenCalls / 5 ) );  // 20% max when falling

    m_iTimeMsBetweenCalls = Clamp ( 1, m_iTimeMsBetweenCalls, 100 );

    // Update each send queue
    for ( uint i = 0 ; i < m_SendQueueList.size () ; i++ )
        m_SendQueueList[i]->DoPulse ( m_iTimeMsBetweenCalls );
}


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::RemoveRemote
//
// When a remote leaves the system
//
///////////////////////////////////////////////////////////////
void CLatentTransferManager::RemoveRemote ( NetPlayerID remoteId )
{
    CLatentSendQueue* pSendQueue = FindSendQueueForRemote ( remoteId );
    CLatentReceiver* pReceiver = FindReceiverForRemote ( remoteId );

    ListRemove ( m_SendQueueList, pSendQueue );
    MapRemove ( m_SendQueueMap, remoteId );
    MapRemove ( m_ReceiverMap, remoteId );

    SAFE_DELETE( pSendQueue );
    SAFE_DELETE( pReceiver );
}


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::OnLuaMainDestroy
//
// When a Lua VM is stopped
//
///////////////////////////////////////////////////////////////
void CLatentTransferManager::OnLuaMainDestroy ( void* pLuaMain )
{
    for ( uint i = 0 ; i < m_SendQueueList.size () ; i++ )
        m_SendQueueList[i]->OnLuaMainDestroy ( pLuaMain );
}


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::AddSendBatchBegin
//
// Prepare data for possible multiple sends
//
///////////////////////////////////////////////////////////////
void CLatentTransferManager::AddSendBatchBegin ( unsigned char ucPacketId, NetBitStreamInterface* pBitStream )
{
    assert ( !m_pBatchBufferRef );

    uint uiBitStreamBitsUsed = pBitStream->GetNumberOfBitsUsed ();
    uint uiBitStreamBytesUsed = ( uiBitStreamBitsUsed + 7 ) >> 3;

    // Make a buffer containing enough info to recreate ucPacketId+BitStream at the other end
    m_pBatchBufferRef = new CBufferRef ( new CBuffer () );

    CBuffer& buffer = **m_pBatchBufferRef;
    CBufferWriteStream stream ( buffer );
    stream.Write ( ucPacketId );
    stream.Write ( uiBitStreamBitsUsed );
    uint uiHeadSize = buffer.GetSize ();

    // Copy data from bitstream into buffer
    buffer.SetSize ( uiHeadSize + uiBitStreamBytesUsed );
    *(buffer.GetData () + buffer.GetSize () - 1) = 0;
    pBitStream->ResetReadPointer ();
    pBitStream->ReadBits ( buffer.GetData () + uiHeadSize, uiBitStreamBitsUsed );
}


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::AddSend
//
// Send to remote
//
///////////////////////////////////////////////////////////////
SSendHandle CLatentTransferManager::AddSend ( NetPlayerID remoteId, ushort usBitStreamVersion, uint uiRate, void* pLuaMain, ushort usResourceNetId )
{
    assert ( m_pBatchBufferRef );

    CLatentSendQueue* pSendQueue = GetSendQueueForRemote ( remoteId, usBitStreamVersion );
    return pSendQueue->AddSend ( *m_pBatchBufferRef, uiRate, CATEGORY_PACKET, pLuaMain, usResourceNetId );
}


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::AddSendBatchEnd
//
// Finished multiple sends
//
///////////////////////////////////////////////////////////////
void CLatentTransferManager::AddSendBatchEnd ( void )
{
    assert ( m_pBatchBufferRef );
    SAFE_DELETE ( m_pBatchBufferRef );
}


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::CancelSend
//
// Returns true if transfer was cancelled.
//
///////////////////////////////////////////////////////////////
bool CLatentTransferManager::CancelSend ( NetPlayerID remoteId, SSendHandle handle )
{
    CLatentSendQueue* pSendQueue = FindSendQueueForRemote ( remoteId );
    if ( !pSendQueue )
        return false;

    return pSendQueue->CancelSend ( handle );
}


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::CancelAllSends
//
//
//
///////////////////////////////////////////////////////////////
void CLatentTransferManager::CancelAllSends ( NetPlayerID remoteId )
{
    CLatentSendQueue* pSendQueue = FindSendQueueForRemote ( remoteId );
    if ( !pSendQueue )
        return;

    return pSendQueue->CancelAllSends ();
}


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::GetSendStatus
//
// Return start and end ticks for the send
//
///////////////////////////////////////////////////////////////
bool CLatentTransferManager::GetSendStatus ( NetPlayerID remoteId, SSendHandle handle, SSendStatus* pOutSendStatus )
{
    CLatentSendQueue* pSendQueue = FindSendQueueForRemote ( remoteId );
    if ( !pSendQueue )
        return false;

    return pSendQueue->GetSendStatus ( handle, pOutSendStatus );
}


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::GetSendHandles
//
//
//
///////////////////////////////////////////////////////////////
bool CLatentTransferManager::GetSendHandles ( NetPlayerID remoteId, std::vector < SSendHandle >& outResultList )
{
    CLatentSendQueue* pSendQueue = FindSendQueueForRemote ( remoteId );
    if ( !pSendQueue )
        return false;

    return pSendQueue->GetSendHandles ( outResultList );
}


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::GetSendQueueForRemote
//
// Find or create
//
///////////////////////////////////////////////////////////////
CLatentSendQueue* CLatentTransferManager::GetSendQueueForRemote ( NetPlayerID remoteId, ushort usBitStreamVersion )
{
    CLatentSendQueue* pSendQueue = MapFindRef ( m_SendQueueMap, remoteId );
    if ( !pSendQueue )
    {
        pSendQueue = new CLatentSendQueue ( remoteId, usBitStreamVersion );
        MapSet ( m_SendQueueMap, remoteId, pSendQueue );
        m_SendQueueList.push_back ( pSendQueue );
    }

    return pSendQueue;
}


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::FindSendQueueForRemote
//
// Find or fail
//
///////////////////////////////////////////////////////////////
CLatentSendQueue* CLatentTransferManager::FindSendQueueForRemote ( NetPlayerID remoteId )
{
    return MapFindRef ( m_SendQueueMap, remoteId );
}


//
//
//
//  Receive
//
//
//


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::OnReceive
//
//
//
///////////////////////////////////////////////////////////////
void CLatentTransferManager::OnReceive ( NetPlayerID remoteId, NetBitStreamInterface* pBitStream )
{
    CLatentReceiver* pReceiver = GetReceiverForRemote ( remoteId, pBitStream->Version () );
    pReceiver->OnReceive ( pBitStream );
}


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::GetReceiverForRemote
//
// Find or create
//
///////////////////////////////////////////////////////////////
CLatentReceiver* CLatentTransferManager::GetReceiverForRemote ( NetPlayerID remoteId, ushort usBitStreamVersion )
{
    CLatentReceiver* pReceiver = MapFindRef ( m_ReceiverMap, remoteId );
    if ( !pReceiver )
    {
        pReceiver = new CLatentReceiver ( remoteId, usBitStreamVersion );
        MapSet ( m_ReceiverMap, remoteId, pReceiver );
    }

    return pReceiver;
}


///////////////////////////////////////////////////////////////
//
// CLatentTransferManager::FindReceiverForRemote
//
// Find or fail
//
///////////////////////////////////////////////////////////////
CLatentReceiver* CLatentTransferManager::FindReceiverForRemote ( NetPlayerID remoteId )
{
    return MapFindRef ( m_ReceiverMap, remoteId );
}




//
//
// Client version of some net functions
//
//

#ifdef MTA_CLIENT

NetBitStreamInterface* DoAllocateNetBitStream ( NetPlayerID remoteId, ushort usBitStreamVersion )
{
    return g_pNet->AllocateNetBitStream ();
}

void DoDeallocateNetBitStream ( NetBitStreamInterface* pBitStream )
{
    return g_pNet->DeallocateNetBitStream ( pBitStream );
}

bool DoSendPacket ( unsigned char ucPacketID, NetPlayerID remoteId, NetBitStreamInterface* bitStream, NetPacketPriority packetPriority, NetPacketReliability packetReliability, ePacketOrdering packetOrdering )
{
    return g_pNet->SendPacket ( ucPacketID, bitStream, packetPriority, packetReliability, packetOrdering );
}

bool DoStaticProcessPacket ( unsigned char ucPacketID, NetPlayerID remoteId, NetBitStreamInterface* pBitStream, ushort usResourceNetId )
{
    // Check if latent packet should be ignored
    if ( usResourceNetId != 0xFFFF )
    {
        CResource* pResource = g_pClientGame->GetResourceManager ()->GetResourceFromNetID ( usResourceNetId );
        if ( !pResource )
            return true;
    }
    return CClientGame::StaticProcessPacket ( ucPacketID, *pBitStream );
}

void DoDisconnectRemote ( NetPlayerID remoteId, const SString& strReason )
{
    g_pCore->ShowMessageBox ( "Error", strReason, MB_BUTTON_OK | MB_ICON_ERROR );
    g_pCore->GetModManager ()->RequestUnload ();
}

#endif


//
//
// Server version of some net functions
//
//

#ifndef MTA_CLIENT

NetBitStreamInterface* DoAllocateNetBitStream ( NetPlayerID remoteId, ushort usBitStreamVersion )
{
    return g_pNetServer->AllocateNetServerBitStream ( usBitStreamVersion );
}

void DoDeallocateNetBitStream ( NetBitStreamInterface* pBitStream )
{
    return g_pNetServer->DeallocateNetServerBitStream ( pBitStream );
}

bool DoSendPacket ( unsigned char ucPacketID, NetPlayerID remoteId, NetBitStreamInterface* bitStream, NetServerPacketPriority packetPriority, NetServerPacketReliability packetReliability, ePacketOrdering packetOrdering  )
{
    return g_pNetServer->SendPacket ( ucPacketID, remoteId, bitStream, FALSE, packetPriority, packetReliability, packetOrdering );
}

bool DoStaticProcessPacket ( unsigned char ucPacketID, NetPlayerID remoteId, NetBitStreamInterface* pBitStream, ushort usResourceNetId )
{
    // Check if latent packet should be ignored
    if ( usResourceNetId != 0xFFFF )
    {
        CResource* pResource = g_pGame->GetResourceManager ()->GetResourceFromNetID ( usResourceNetId );
        if ( !pResource )
            return true;
    }
    return CGame::StaticProcessPacket ( ucPacketID, remoteId, pBitStream, NULL );
}

void DoDisconnectRemote ( NetPlayerID remoteId, const SString& strReason )
{
    CPlayer* pPlayer = g_pGame->GetPlayerManager ()->Get ( remoteId );
    if ( pPlayer )
        DisconnectPlayer ( g_pGame, *pPlayer, strReason );
}

#endif
