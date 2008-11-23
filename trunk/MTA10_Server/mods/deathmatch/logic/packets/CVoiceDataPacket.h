/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CVoiceDataPacket.h
*  PURPOSE:     Voice data packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
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
                            CVoiceDataPacket            ( void );
                           ~CVoiceDataPacket            ( );

    ePacketID               GetPacketID                 ( void ) const;
    unsigned long           GetFlags                    ( void ) const;

    bool                    Read                        ( NetServerBitStreamInterface& BitStream );
    bool                    Write                       ( NetServerBitStreamInterface& BitStream ) const;

    ElementID               GetPlayer                   ( void );
    void                    SetPlayer                   ( ElementID PlayerID );

    void                    SetData                     ( const unsigned char * pbSrcBuffer, unsigned short usLength );

    unsigned short          GetDataLength               ( ) const;
    const unsigned char *   GetData                     ( ) const;

private:
    void                    AllocateBuffer              ( unsigned short usBufferSize );
    void                    DeallocateBuffer            ( );

    ElementID               m_PlayerID;
    unsigned char *         m_pbDataBuffer;
    unsigned short          m_usDataBufferSize;
    unsigned short          m_usActualDataLength;
};

#endif
