/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerClothesPacket.cpp
*  PURPOSE:     Player clothes packet class
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPlayerClothesPacket::~CPlayerClothesPacket ( void )
{
    vector < SPlayerClothes* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end () ; iter++ )
    {
        delete [] (*iter)->szTexture;
        delete [] (*iter)->szModel;
        delete *iter;
    }
    m_List.clear ();
}


bool CPlayerClothesPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Write the source player.
    if ( m_pSourceElement )
    {
        ElementID ID = m_pSourceElement->GetID ();
        BitStream.Write ( ID );

        // Write the clothes
        unsigned short usNumClothes = static_cast < unsigned short > ( m_List.size () );
        BitStream.Write ( usNumClothes );

        vector < SPlayerClothes* > ::const_iterator iter = m_List.begin ();
        for ( ; iter != m_List.end () ; iter++ )
        {
            char* szTexture = (*iter)->szTexture;
            char* szModel = (*iter)->szModel;
            unsigned char ucTextureLength = strlen ( szTexture );
            unsigned char ucModelLength = strlen ( szModel );
            BitStream.Write ( ucTextureLength );
            BitStream.Write ( szTexture, ucTextureLength );
            BitStream.Write ( ucModelLength );
            BitStream.Write ( szModel, ucModelLength );
            BitStream.Write ( (*iter)->ucType );
        }

        return true;
    }

    return false;
}


void CPlayerClothesPacket::Add ( const char* szTexture, const char* szModel, unsigned char ucType )
{
    SPlayerClothes* Clothes = new SPlayerClothes;
    Clothes->szTexture = new char [ strlen ( szTexture ) + 1 ];
    strcpy ( Clothes->szTexture, szTexture );
    Clothes->szModel = new char [ strlen ( szModel ) + 1 ];
    strcpy ( Clothes->szModel, szModel );
    Clothes->ucType = ucType;
    m_List.push_back ( Clothes );
}


void CPlayerClothesPacket::Add ( CPlayerClothes* pClothes )
{
    for ( unsigned char ucType = 0 ; ucType < PLAYER_CLOTHING_SLOTS ; ucType++ )
    {
        SPlayerClothing* pClothing = pClothes->GetClothing ( ucType );
        if ( pClothing )
        {
            Add ( pClothing->szTexture, pClothing->szModel, ucType );
        }
    }
}

