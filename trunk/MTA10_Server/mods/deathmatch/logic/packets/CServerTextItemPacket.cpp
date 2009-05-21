/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CServerTextItemPacket.cpp
*  PURPOSE:     Server text item packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Kent Simon <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CServerTextItemPacket::CServerTextItemPacket( void )
{
    m_szText = NULL;
}


CServerTextItemPacket::CServerTextItemPacket( unsigned long ulUniqueId, bool bDeleteable, float fX, float fY, float fScale, unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha, unsigned char format, char* szText )
{
    m_ulUniqueId = ulUniqueId;
    m_bDeletable = bDeleteable;
    m_fX = fX;
    m_fY = fY;
    m_fScale = fScale;
    m_red = red;
    m_green = green;
    m_blue = blue;
    m_alpha = alpha;
    m_ucFormat = format;
    m_szText = new char [strlen ( szText ) + 1];
    strcpy ( m_szText, szText );
}


CServerTextItemPacket::~CServerTextItemPacket ( void )
{
    if ( m_szText )
    {
        delete [] m_szText;
    }
}


bool CServerTextItemPacket::Write ( NetBitStreamInterface &BitStream  ) const
{
    BitStream.Write ( m_ulUniqueId );

    // Grab the flag byte
    unsigned char ucFlags = 0;
    ucFlags |= m_bDeletable ? 1:0;

    // Write the flag byte
    BitStream.Write ( ucFlags );

    // Not deleting this?
    if ( !m_bDeletable )
    {
        BitStream.Write ( m_fX );
        BitStream.Write ( m_fY );
        BitStream.Write ( m_fScale );
        BitStream.Write ( m_red );
        BitStream.Write ( m_green );
        BitStream.Write ( m_blue );
        BitStream.Write ( m_alpha );
        BitStream.Write ( m_ucFormat );

        // Grab the text length
        size_t sizeText = strlen ( m_szText );
        if ( sizeText > 1024 )
        {
            sizeText = 1024;
        }

        // Write the text
        BitStream.Write ( static_cast < unsigned short > ( sizeText ) );
        if ( sizeText )
        {
            BitStream.Write ( m_szText, sizeText );
        }
    }

    return true;
}
