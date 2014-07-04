/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVoiceDataPacket.cpp
*  PURPOSE:     Voice data packet class
*  DEVELOPERS:  Philip Farquharson <philip@philipfarquharson.co.uk>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CVoiceDataPacket::CVoiceDataPacket ( void )
{
    m_pBuffer = NULL;
    m_usDataBufferSize = 0;
    m_usActualDataLength = 0;

    AllocateBuffer ( 1024 );
}

CVoiceDataPacket::CVoiceDataPacket( CPlayer* pPlayer, const unsigned char * pbSrcBuffer, unsigned short usLength )
{
    m_pBuffer = NULL;
    m_usDataBufferSize = 0;
    m_usActualDataLength = 0;

    SetSourceElement ( pPlayer );
    SetData(pbSrcBuffer, usLength);
}
CVoiceDataPacket::~CVoiceDataPacket ( )
{
    DeallocateBuffer ( );
}


bool CVoiceDataPacket::Read ( NetBitStreamInterface& BitStream )
{
    if ( m_pBuffer )
    {
        BitStream.Read ( m_usActualDataLength );
        if ( m_usActualDataLength )
        {
            BitStream.Read ( reinterpret_cast < char * > ( m_pBuffer ), m_usActualDataLength <= m_usDataBufferSize ? 
                                                                            m_usActualDataLength : m_usDataBufferSize );
        }
        return true;
    }

    return false;
}


bool CVoiceDataPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    if ( m_usActualDataLength )
    {
            // Write the source player
            ElementID ID = m_pSourceElement->GetID();
            BitStream.Write ( ID );
            // Write the length as an unsigned short and then write the string
            BitStream.Write ( m_usActualDataLength );
            BitStream.Write ( reinterpret_cast < char * > ( m_pBuffer ), m_usActualDataLength );
            return true;
    }

    return false;
}

void CVoiceDataPacket::AllocateBuffer ( unsigned short usBufferSize )
{
    // Test to see if we already have an allocated buffer
    // that will hold the requested size.
    if ( m_usDataBufferSize < usBufferSize )
    {
        // It's not... resize the buffer.
        if ( m_pBuffer )
        {
            // Free the current buffer.
            delete [] m_pBuffer;
        }
        
        // Allocate new buffer.
        m_pBuffer = new unsigned char [ usBufferSize ];

        // Clear buffer.
        memset ( m_pBuffer, 0, usBufferSize );

        // Save the new size.
        m_usDataBufferSize = usBufferSize;
    }
}

void CVoiceDataPacket::DeallocateBuffer ( )
{
    if ( m_pBuffer )
    {
        delete [ ] m_pBuffer;
        m_pBuffer = NULL;
        m_usDataBufferSize = 0;
        m_usActualDataLength = 0;
    }
}

void CVoiceDataPacket::SetData ( const unsigned char * pbSrcBuffer, unsigned short usLength )
{
    // Allocate new buffer.
    AllocateBuffer ( usLength );

    // Copy in the data.
    if ( m_pBuffer )
    {
        memcpy ( m_pBuffer, pbSrcBuffer, usLength );
        m_usActualDataLength = usLength;
    }
}

const unsigned char * CVoiceDataPacket::GetData ( ) const
{
    return m_pBuffer;
}

unsigned short CVoiceDataPacket::GetDataLength ( ) const
{
    return m_usActualDataLength;
}