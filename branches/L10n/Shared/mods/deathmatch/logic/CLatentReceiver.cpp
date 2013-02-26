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
// CLatentReceiver::CLatentReceiver
//
//
//
///////////////////////////////////////////////////////////////
CLatentReceiver::CLatentReceiver ( NetPlayerID remoteId, ushort usBitStreamVersion )
    : m_RemoteId ( remoteId )
    , m_usBitStreamVersion ( usBitStreamVersion )
{
}


///////////////////////////////////////////////////////////////
//
// CLatentReceiver::~CLatentReceiver
//
//
//
///////////////////////////////////////////////////////////////
CLatentReceiver::~CLatentReceiver ( void )
{
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
// CLatentReceiver::OnReceiveError
//
// Receive error is fatal for the connection
//
///////////////////////////////////////////////////////////////
void CLatentReceiver::OnReceiveError ( const SString& strMessage )
{
    DoDisconnectRemote ( m_RemoteId, SString ( "Latent receiver error (%s)", *strMessage ) );
}


///////////////////////////////////////////////////////////////
//
// CLatentReceiver::OnReceive
//
//
//
///////////////////////////////////////////////////////////////
void CLatentReceiver::OnReceive ( NetBitStreamInterface* pBitStream )
{
    //
    // Read header
    //
    ushort usId = 0;
    bool bIsHead = false;
    bool bIsTail = false;
    bool bIsCancel = false;
    ushort usCategory = 0;
    ushort usResourceNetId = 0xFFFF;
    uint uiFinalSize = 0;
    uint uiRate = 0;

    pBitStream->ReadBits ( &usId, 15 );
    if ( pBitStream->ReadBit () )
    {
        // Special type
        uchar ucSpecialFlag;
        pBitStream->Read ( ucSpecialFlag );
        if ( ucSpecialFlag == FLAG_HEAD )
        {
            bIsHead = true;
            pBitStream->Read ( usCategory );
            pBitStream->Read ( uiFinalSize );
            pBitStream->Read ( uiRate );
            if ( pBitStream->Version () >= 0x31 )
                pBitStream->Read ( usResourceNetId );
        }
        else
        if ( ucSpecialFlag == FLAG_TAIL )
        {
            bIsTail = true;
        }
        else
        if ( ucSpecialFlag == FLAG_CANCEL )
        {
            bIsCancel = true;
        }
        else
        {
            return OnReceiveError ( "Invalid special type" );
        }
    }

    pBitStream->AlignReadToByteBoundary ();
    ushort usSizeSent = 0;
    pBitStream->Read ( usSizeSent );

    //
    // Process header
    //
    if ( bIsHead )
    {
        // If head, check no previous transfer
        if ( activeRx.bReceiveStarted )
            return OnReceiveError ( "bIsHead && activeRx.bReceiveActive" );
        if ( uiFinalSize > 100 * 1024 * 1024 )
            return OnReceiveError ( "uiFinalSize too large" );

        activeRx.usId = usId;
        activeRx.bReceiveStarted = true;
        activeRx.usCategory = usCategory;
        activeRx.uiRate = uiRate;
        activeRx.usResourceNetId = usResourceNetId;
        activeRx.buffer.SetSize ( uiFinalSize );
        activeRx.uiWritePosition = 0;
    }

    if ( activeRx.usId != usId )
        return OnReceiveError ( "usId wrong" );

    if ( bIsCancel )
    {
        // Reset for next receive
        activeRx = SReceiveItem ();
        return;
    }


    //
    // Read body
    //
    if ( activeRx.uiWritePosition + usSizeSent > activeRx.buffer.GetSize () )
        return OnReceiveError ( "Buffer would overflow" );

    if ( bIsTail && activeRx.uiWritePosition + usSizeSent != activeRx.buffer.GetSize () )
        return OnReceiveError ( "Buffer size wrong" );

    pBitStream->Read ( activeRx.buffer.GetData () + activeRx.uiWritePosition, usSizeSent );
    activeRx.uiWritePosition += usSizeSent;


    //
    // Process tail
    //
    if ( bIsTail )
    {
        if ( activeRx.usCategory == CATEGORY_PACKET )
        {
            // Recreate the packet data
            NetBitStreamInterface* pBitStream = DoAllocateNetBitStream ( m_RemoteId, m_usBitStreamVersion );
            uchar ucPacketId = 0;
            uint uiBitStreamBitsUsed = 0;

            CBufferReadStream stream ( activeRx.buffer );
            stream.Read ( ucPacketId );
            stream.Read ( uiBitStreamBitsUsed );
            uint uiBitStreamBytesUsed = ( uiBitStreamBitsUsed + 7 ) >> 3;

            if ( uiBitStreamBytesUsed != activeRx.buffer.GetSize () - 5 )
                return OnReceiveError ( "Buffer size mismatch" );

            pBitStream->WriteBits ( activeRx.buffer.GetData () + 5, uiBitStreamBitsUsed );
            pBitStream->ResetReadPointer ();

            DoStaticProcessPacket ( ucPacketId, m_RemoteId, pBitStream, activeRx.usResourceNetId );
            DoDeallocateNetBitStream ( pBitStream );
        }
        else
        {
            OutputDebugLine ( "[Misc] CLatentReceiver::OnReceive - Unknown category" );
        }

        // Reset for next receive
        activeRx = SReceiveItem ();
    }
}
