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


CServerTextItemPacket::CServerTextItemPacket( unsigned long ulUniqueId, bool bDeleteable, float fX, float fY, float fScale, const SColor color, unsigned char format, unsigned char ucShadowAlpha, char* szText )
{
    m_ulUniqueId = ulUniqueId;
    m_bDeletable = bDeleteable;
    m_fX = fX;
    m_fY = fY;
    m_fScale = fScale;
    m_Color = color;
    m_ucFormat = format;
    m_ucShadowAlpha = ucShadowAlpha;
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
    BitStream.WriteCompressed ( m_ulUniqueId );

    // Write the flag byte
    BitStream.WriteBit ( m_bDeletable );

    // Not deleting this?
    if ( !m_bDeletable )
    {
        BitStream.Write ( m_fX );
        BitStream.Write ( m_fY );
        BitStream.Write ( m_fScale );
        BitStream.Write ( m_Color.R );
        BitStream.Write ( m_Color.G );
        BitStream.Write ( m_Color.B );
        BitStream.Write ( m_Color.A );
        BitStream.Write ( m_ucFormat );
        if ( BitStream.Version() >= 0x03 )
            BitStream.Write ( m_ucShadowAlpha );

        // Grab the text length
        size_t sizeText = strlen ( m_szText );
        if ( sizeText > 1024 )
        {
            sizeText = 1024;
        }

        // Write the text
        BitStream.WriteCompressed ( static_cast < unsigned short > ( sizeText ) );
        if ( sizeText )
        {
            BitStream.Write ( m_szText, sizeText );
        }
    }

    return true;
}
