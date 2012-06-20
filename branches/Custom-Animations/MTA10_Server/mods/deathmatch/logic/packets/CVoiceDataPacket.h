/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVoiceDataPacket.h
*  PURPOSE:     Voice data packet class
*  DEVELOPERS:  Philip Farquharson <philip@philipfarquharson.co.uk>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CVOICEDATAPACKET_H
#define __PACKETS_CVOICEDATAPACKET_H

#include "CPacket.h"

class CVoiceDataPacket : public CPacket
{
public:
                            CVoiceDataPacket            ( CPlayer* pPlayer, const unsigned char * pbSrcBuffer, unsigned short usLength );
                            CVoiceDataPacket            ( void );
                           ~CVoiceDataPacket            ( );

    ePacketID               GetPacketID                 ( void ) const              { return PACKET_ID_VOICE_DATA; }
    unsigned long           GetFlags                    ( void ) const              { return PACKET_LOW_PRIORITY | PACKET_SEQUENCED; };
    virtual ePacketOrdering GetPacketOrdering           ( void ) const              { return PACKET_ORDERING_VOICE; }

    bool                    Read                        ( NetBitStreamInterface& BitStream );
    bool                    Write                       ( NetBitStreamInterface& BitStream ) const;

    void                    SetData                     ( const unsigned char * pbSrcBuffer, unsigned short usLength );

    unsigned short          GetDataLength               ( ) const;
    const unsigned char *   GetData                     ( ) const;

private:
    void                    AllocateBuffer              ( unsigned short usBufferSize );
    void                    DeallocateBuffer            ( );

    unsigned char *         m_pBuffer;
    unsigned short          m_usDataBufferSize;
    unsigned short          m_usActualDataLength;
};

#endif
