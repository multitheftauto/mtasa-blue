/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerClothesPacket.h
*  PURPOSE:     Player clothes packet class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __PACKETS_CPLAYERCLOTHESPACKET_H
#define __PACKETS_CPLAYERCLOTHESPACKET_H

#include "CPacket.h"
#include "../CPlayerClothes.h"

struct SPlayerClothes
{
    char* szTexture;
    char* szModel;
    unsigned char ucType;
};

class CPlayerClothesPacket: public CPacket
{
public:
                                ~CPlayerClothesPacket       ( void );

    inline ePacketID			GetPacketID                 ( void ) const                  { return PACKET_ID_PLAYER_CLOTHES; };
    inline unsigned long		GetFlags                    ( void ) const                  { return PACKET_RELIABLE | PACKET_SEQUENCED; };

    bool						Write                       ( NetBitStreamInterface& BitStream ) const;

	void						Add							( char* szTexture, char* szModel, unsigned char ucType );
    void                        Add                         ( CPlayerClothes* pClothes );
    inline unsigned int         Count                       ( void )                        { return static_cast < unsigned int > ( m_List.size () ); }

private:
	vector < SPlayerClothes* >	m_List;
};

#endif
