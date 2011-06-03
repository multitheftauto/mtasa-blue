/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVoiceDataPacket.cpp
*  PURPOSE:     Voice data packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CVoiceDataPacket::CVoiceDataPacket ( void )
{
    m_PlayerID = INVALID_ELEMENT_ID;
    m_pbDataBuffer = NULL;
    m_usDataBufferSize = 0;
    m_usActualDataLength = 0;

    AllocateBuffer ( 1024 );
}

CVoiceDataPacket::~CVoiceDataPacket ( )
{
    DeallocateBuffer ( );
}


ePacketID CVoiceDataPacket::GetPacketID ( void ) const
{
    return static_cast < ePacketID > ( PACKET_ID_VOICE_DATA );
}


unsigned long CVoiceDataPacket::GetFlags ( void ) const
{
    return 0;
}


bool CVoiceDataPacket::Read ( NetBitStreamInterface& BitStream )
{
    if ( m_pbDataBuffer )
    {
        BitStream.Read ( m_usActualDataLength );
        if ( m_usActualDataLength )
        {
            BitStream.Read ( reinterpret_cast < char * > ( m_pbDataBuffer ), m_usActualDataLength <= m_usDataBufferSize ? 
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
            // Write the length as an unsigned short and then write the string
            BitStream.Write ( m_usActualDataLength );
            BitStream.Write ( reinterpret_cast < char * > ( m_pbDataBuffer ), m_usActualDataLength );
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
        if ( m_pbDataBuffer )
        {
            // Free the current buffer.
            delete [] m_pbDataBuffer;
        }
        
        // Allocate new buffer.
        m_pbDataBuffer = new unsigned char [ usBufferSize ];

        // Clear buffer.
        memset ( m_pbDataBuffer, 0, usBufferSize );

        // Save the new size.
        m_usDataBufferSize = usBufferSize;
    }
}

void CVoiceDataPacket::DeallocateBuffer ( )
{
    if ( m_pbDataBuffer )
    {
        delete [ ] m_pbDataBuffer;
        m_pbDataBuffer = NULL;
        m_usDataBufferSize = 0;
        m_usActualDataLength = 0;
    }
}

void CVoiceDataPacket::SetData ( const unsigned char * pbSrcBuffer, unsigned short usLength )
{
    // Allocate new buffer.
    AllocateBuffer ( usLength );

    // Copy in the data.
    if ( m_pbDataBuffer )
    {
        memcpy ( m_pbDataBuffer, pbSrcBuffer, usLength );
        m_usActualDataLength = usLength;
    }
}

const unsigned char * CVoiceDataPacket::GetData ( ) const
{
    return m_pbDataBuffer;
}

unsigned short CVoiceDataPacket::GetDataLength ( ) const
{
    return m_usActualDataLength;
}
